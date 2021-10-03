#include "assetsdownloadoperation.h"

#include <QFile>
#include <QTimer>
#include <QBuffer>
#include <QStandardPaths>

#include "flipperzero/cli/removeoperation.h"
#include "flipperzero/cli/mkdiroperation.h"
#include "flipperzero/cli/writeoperation.h"
#include "flipperzero/cli/readoperation.h"
#include "flipperzero/cli/statoperation.h"
#include "flipperzero/commandinterface.h"
#include "flipperzero/assetmanifest.h"
#include "flipperzero/flipperzero.h"
#include "gzipuncompressor.h"
#include "tararchive.h"

#include "macros.h"

#define RESOURCES_PREFIX QByteArrayLiteral("resources")
#define DEVICE_MANIFEST QByteArrayLiteral("/ext/Manifest")

using namespace Flipper;
using namespace Zero;

static void print_file_list(const QString &header, const FileNode::FileInfoList &list)
{
    qDebug() << header;

    for(const auto &e : list) {
        const auto icon = QStringLiteral("[%1]").arg(e.type == FileNode::Type::RegularFile ? 'F' :
                                                     e.type == FileNode::Type::Directory   ? 'D' : '?');
        qDebug() << icon << e.absolutePath;
    }
}

AssetsDownloadOperation::AssetsDownloadOperation(FlipperZero *device, QIODevice *file, QObject *parent):
    FlipperZeroOperation(device, parent),
    m_compressed(file),
    m_uncompressed(nullptr),
    m_isDeviceManifestPresent(false)
{
    device->setMessage(QStringLiteral("Databases download pending..."));
}

AssetsDownloadOperation::~AssetsDownloadOperation()
{}

const QString AssetsDownloadOperation::description() const
{
    return QStringLiteral("Assets Download @%1 %2").arg(device()->model(), device()->name());
}

void AssetsDownloadOperation::transitionToNextState()
{
    if(operationState() == BasicOperationState::Ready) {
        setOperationState(State::CheckingExtStorage);
        if(!checkForExtStorage()) {
            finishWithError(QStringLiteral("Failed to access the external storage"));
        }

    } else if(operationState() == State::CheckingExtStorage) {
        setOperationState(State::ExtractingArchive);
        if(!extractArchive()) {
            finishWithError(QStringLiteral("Failed to extract the databases archive"));
        }

    } else if(operationState() == State::ExtractingArchive) {
        setOperationState(ReadingLocalManifest);
        if(!readLocalManifest()) {
            finishWithError(QStringLiteral("Failed to read local manifest"));
        }

    } else if(operationState() == State::ReadingLocalManifest) {
        setOperationState(CheckingDeviceManifest);
        if(!checkForDeviceManifest()) {
            finishWithError(QStringLiteral("Failed to check for device manifest"));
        }

    } else if(operationState() == State::CheckingDeviceManifest) {
        setOperationState(ReadingDeviceManifest);
        if(!readDeviceManifest()) {
            finishWithError(QStringLiteral("Failed to read device manifest"));
        }

    } else if(operationState() == State::ReadingDeviceManifest) {
        setOperationState(State::BuildingFileLists);
        if(!buildFileLists()) {
            finishWithError(QStringLiteral("Failed to build file lists"));
        }

    } else if(operationState() == State::BuildingFileLists) {
        setOperationState(State::DeletingFiles);
        if(!deleteFiles()) {
            finishWithError(QStringLiteral("Failed to delete files"));
        }

    } else if(operationState() == State::DeletingFiles) {
        setOperationState(State::WritingFiles);
        if(!writeFiles()) {
            finishWithError(QStringLiteral("Failed to write files"));
        }

    } else if(operationState() == State::WritingFiles) {
        finish();
    }

    if(isError()) {
        device()->setError(errorString());
    }
}

void AssetsDownloadOperation::onOperationTimeout()
{
    qDebug() << "Operation timeout";
}

bool AssetsDownloadOperation::checkForExtStorage()
{
    auto *op = device()->cli()->stat(QByteArrayLiteral("/ext"));

    connect(op, &AbstractOperation::finished, this, [=]() {
        if(op->isError()) {
            finishWithError("Failed to perform stat operation");
        } else if(op->type() == StatOperation::Type::InternalError) {
            info_msg("No external storage found, finishing early.");
            finish();

        } else if(op->type() != StatOperation::Type::Storage) {
            finishWithError("/ext is not a storage");
        } else {
            info_msg(QStringLiteral("External storage is present, %1 bytes free.").arg(op->sizeFree()));
            transitionToNextState();
        }

        if(isError()) {
            device()->setError(errorString());
        }

        op->deleteLater();
    });

    return true;
}

bool AssetsDownloadOperation::extractArchive()
{
    // TODO: put everything into a directory & generate random names for the files
    const auto tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    check_return_bool(!tempPath.isEmpty(), "Failed to find a suitable temporary location.");

    check_return_bool(m_compressed->open(QIODevice::ReadOnly), m_compressed->errorString());

    // TODO: check if file exists, etc.
    m_uncompressed = new QFile(tempPath + "/qflipper-databases.tar", this);
    check_return_bool(m_uncompressed->open(QIODevice::ReadWrite), m_uncompressed->errorString());

    auto *uncompressor = new GZipUncompressor(m_compressed, m_uncompressed, this);

    connect(uncompressor, &GZipUncompressor::finished, this, [=]() {
        if(uncompressor->isError()) {
            finishWithError(uncompressor->errorString());
        } else {
            m_archive = TarArchive(m_uncompressed);

            if(m_archive.isError()) {
                finishWithError(m_archive.errorString());
            } else {
                QTimer::singleShot(0, this, &AssetsDownloadOperation::transitionToNextState);
            }
        }

        uncompressor->deleteLater();
        m_compressed->deleteLater();
    });

    return true;
}

bool AssetsDownloadOperation::readLocalManifest()
{
    const auto text = m_archive.fileData(QStringLiteral("resources/Manifest"));
    check_return_bool(!text.isEmpty(), m_archive.errorString());

    m_localManifest = AssetManifest(text);
    check_return_bool(!m_localManifest.isError(), m_localManifest.errorString());

    QTimer::singleShot(0, this, &AssetsDownloadOperation::transitionToNextState);
    return true;
}

bool AssetsDownloadOperation::checkForDeviceManifest()
{
    auto *op = device()->cli()->stat(DEVICE_MANIFEST);
    connect(op, &AbstractOperation::finished, this, [=]() {
        if(op->isError()) {
            finishWithError(op->errorString());
        } else {
            if(op->type() != StatOperation::Type::RegularFile) {
                setOperationState(State::ReadingDeviceManifest);
            } else {
                m_isDeviceManifestPresent = true;
            }

            QTimer::singleShot(0, this, &AssetsDownloadOperation::transitionToNextState);
        }

        op->deleteLater();
    });

    return true;
}

bool AssetsDownloadOperation::readDeviceManifest()
{
    auto *buf = new QBuffer(this);

    if(!buf->open(QIODevice::ReadWrite)) {
        buf->deleteLater();
        return false;
    }

    auto *op = device()->cli()->read(QByteArrayLiteral("/ext/Manifest"), buf);

    connect(op, &AbstractOperation::finished, this, [=]() {
        if(!op->isError()) {
            m_deviceManifest = AssetManifest(buf->readAll());
        }

        op->deleteLater();
        buf->deleteLater();

        QTimer::singleShot(0, this, &AssetsDownloadOperation::transitionToNextState);
    });

    return true;
}

bool AssetsDownloadOperation::buildFileLists()
{
    FileNode::FileInfoList deleted, added, changed;

    FileNode::FileInfo manifestInfo;
    manifestInfo.name = QStringLiteral("Manifest");
    manifestInfo.absolutePath = manifestInfo.name;
    manifestInfo.type = FileNode::Type::RegularFile;

    print_file_list("<<<<< Local manifest:", m_localManifest.tree()->toPreOrderList());

    if(!m_isDeviceManifestPresent) {
        info_msg("Device manifest not present, assumimg fresh install...");
        added.append(manifestInfo);
        added.append(m_localManifest.tree()->toPreOrderList().mid(1));

    } else if(m_deviceManifest.isError()) {
        info_msg("Failed to build device manifest, assuming complete asset overwrite...");
        changed.append(manifestInfo);
        changed.append(m_localManifest.tree()->toPreOrderList().mid(1));

    } else {
        print_file_list(">>>>> Device manifest:", m_deviceManifest.tree()->toPreOrderList());

        deleted.append(m_localManifest.tree()->difference(m_deviceManifest.tree()));
        added.append(m_deviceManifest.tree()->difference(m_localManifest.tree()));
        changed.append(m_deviceManifest.tree()->changed(m_localManifest.tree()));

        if(!deleted.isEmpty() || !added.isEmpty() || !changed.isEmpty()) {
            changed.prepend(manifestInfo);
        }
    }

    if(!deleted.isEmpty()) {
        print_file_list("----- Files deleted:", deleted);
    }

    if(!added.isEmpty()) {
        print_file_list("+++++ Files added:", added);
    }

    if(!changed.isEmpty()) {
        print_file_list("***** Files changed:", changed);
    }

    m_delete.append(deleted);
    m_delete.append(changed);

    m_write.append(added);
    m_write.append(changed);

    // Start deleting by the farthest nodes
    std::reverse(m_delete.begin(), m_delete.end());

    QTimer::singleShot(0, this, &AssetsDownloadOperation::transitionToNextState);
    return true;
}

bool AssetsDownloadOperation::deleteFiles()
{
    if(m_delete.isEmpty()) {
        info_msg("No files to delete, skipping to write");
        QTimer::singleShot(0, this, &AssetsDownloadOperation::transitionToNextState);
        return true;
    }

    device()->setMessage(tr("Deleting unneeded files..."));

    int numFiles = m_delete.size();

    for(const auto &fileInfo : qAsConst(m_delete)) {
        const auto isLastFile = (--numFiles == 0);
        const auto fileName = QByteArrayLiteral("/ext/") + fileInfo.absolutePath.toLocal8Bit();

        auto *op = device()->cli()->remove(fileName);

        connect(op, &AbstractOperation::finished, this, [=]() {
            if(op->isError()) {
                finishWithError(op->errorString());
            } else if(isLastFile) {
                QTimer::singleShot(0, this, &AssetsDownloadOperation::transitionToNextState);
            }
        });
    }

    return true;
}

bool AssetsDownloadOperation::writeFiles()
{
    if(m_write.isEmpty()) {
        info_msg("No files to write, skipping to the end");
        QTimer::singleShot(0, this, &AssetsDownloadOperation::transitionToNextState);
        return true;
    }


    device()->setMessage(tr("Writing new files..."));

    int i = m_write.size();

    for(const auto &fileInfo : qAsConst(m_write)) {
        --i;

        AbstractOperation *op;
        const auto filePath = QByteArrayLiteral("/ext/") + fileInfo.absolutePath.toLocal8Bit();

        if(fileInfo.type == FileNode::Type::Directory) {
            op = device()->cli()->mkdir(filePath);

        } else if(fileInfo.type == FileNode::Type::RegularFile) {
            auto *buf = new QBuffer(this);
            if(!buf->open(QIODevice::ReadWrite)) {
                buf->deleteLater();
                return false;
            }

            const auto resourcePath = QStringLiteral("resources/") + fileInfo.absolutePath;
            if((buf->write(m_archive.fileData(resourcePath)) <= 0) || (!buf->seek(0))) {
                buf->deleteLater();
                return false;
            }

            op = device()->cli()->write(filePath, buf);

        } else {
            return false;
        }

        connect(op, &AbstractOperation::finished, this, [=]() {
            if(op->isError()) {
                finishWithError(op->errorString());
            } else if(i == 0) {
                QTimer::singleShot(0, this, &AssetsDownloadOperation::transitionToNextState);
            }

            op->deleteLater();
        });
    }

    return true;
}

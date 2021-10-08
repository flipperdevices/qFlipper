#include "assetsdownloadoperation.h"

#include <QDir>
#include <QFile>
#include <QTimer>
#include <QBuffer>

#include "flipperzero/cli/removeoperation.h"
#include "flipperzero/cli/mkdiroperation.h"
#include "flipperzero/cli/writeoperation.h"
#include "flipperzero/cli/readoperation.h"
#include "flipperzero/cli/statoperation.h"

#include "flipperzero/commandinterface.h"
#include "flipperzero/assetmanifest.h"
#include "flipperzero/devicestate.h"

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

AssetsDownloadOperation::AssetsDownloadOperation(CommandInterface *cli, DeviceState *deviceState, QIODevice *compressedFile, QObject *parent):
    AbstractUtilityOperation(cli, deviceState, parent),
    m_compressedFile(compressedFile),
    m_uncompressedFile(nullptr),
    m_isDeviceManifestPresent(false)
{}

AssetsDownloadOperation::~AssetsDownloadOperation()
{}

const QString AssetsDownloadOperation::description() const
{
    const auto &model = deviceState()->deviceInfo().model;
    const auto &name = deviceState()->deviceInfo().name;

    return QStringLiteral("Assets Download @%1 %2").arg(model, name);
}

void AssetsDownloadOperation::advanceOperationState()
{
    if(operationState() == BasicOperationState::Ready) {
        setOperationState(State::CheckingExtStorage);
        checkForExtStorage();

    } else if(operationState() == State::CheckingExtStorage) {
        setOperationState(State::ExtractingArchive);
        extractArchive();

    } else if(operationState() == State::ExtractingArchive) {
        setOperationState(ReadingLocalManifest);
        readLocalManifest();

    } else if(operationState() == State::ReadingLocalManifest) {
        setOperationState(CheckingDeviceManifest);
        checkForDeviceManifest();

    } else if(operationState() == State::CheckingDeviceManifest) {
        setOperationState(ReadingDeviceManifest);
        readDeviceManifest();

    } else if(operationState() == State::ReadingDeviceManifest) {
        setOperationState(State::BuildingFileLists);
        buildFileLists();

    } else if(operationState() == State::BuildingFileLists) {
        setOperationState(State::DeletingFiles);
        deleteFiles();

    } else if(operationState() == State::DeletingFiles) {
        setOperationState(State::WritingFiles);
        writeFiles();

    } else if(operationState() == State::WritingFiles) {
        // TODO: move this into a function for consistency?
        m_uncompressedFile->remove();
        finish();
    }
}

void AssetsDownloadOperation::checkForExtStorage()
{
    auto *op = cli()->stat(QByteArrayLiteral("/ext"));

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
            advanceOperationState();
        }
    });
}

void AssetsDownloadOperation::extractArchive()
{
    // TODO: Use a unified temp path program-wide
    auto tempDir = QDir::temp();
    const auto subdirName = QStringLiteral("qFlipper");

    bool success;

    if(tempDir.exists(subdirName)) {
        success = tempDir.cd(subdirName);
    } else {
        success = tempDir.mkdir(subdirName) && tempDir.cd(subdirName);
    }

    if(!success) {
        return finishWithError(QStringLiteral("Failed to access the temporary directory"));
    } else if(!m_compressedFile->open(QIODevice::ReadOnly)) {
        return finishWithError(m_compressedFile->errorString());
    }

    // TODO: check if file exists, etc.
    m_uncompressedFile = new QFile(tempDir.absoluteFilePath(QStringLiteral("qFlipper-databases.tar")), this);
    if(!m_uncompressedFile->open(QIODevice::ReadWrite)) {
        return finishWithError(m_uncompressedFile->errorString());
    }

    auto *uncompressor = new GZipUncompressor(m_compressedFile, m_uncompressedFile, this);

    connect(uncompressor, &GZipUncompressor::finished, this, [=]() {
        if(uncompressor->isError()) {
            finishWithError(uncompressor->errorString());
        } else {
            m_archive = TarArchive(m_uncompressedFile);

            if(m_archive.isError()) {
                finishWithError(m_archive.errorString());
            } else {
                QTimer::singleShot(0, this, &AssetsDownloadOperation::advanceOperationState);
            }
        }
    });
}

void AssetsDownloadOperation::readLocalManifest()
{
    const auto text = m_archive.fileData(QStringLiteral("resources/Manifest"));
    if(text.isEmpty()) {
        return finishWithError(m_archive.errorString());
    }

    m_localManifest = AssetManifest(text);
    if(m_localManifest.isError()) {
        return finishWithError(m_localManifest.errorString());
    }

    QTimer::singleShot(0, this, &AssetsDownloadOperation::advanceOperationState);
}

void AssetsDownloadOperation::checkForDeviceManifest()
{
    auto *operation = cli()->stat(DEVICE_MANIFEST);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            return finishWithError(operation->errorString());
        } else if(operation->type() != StatOperation::Type::RegularFile) {
            setOperationState(State::ReadingDeviceManifest);
        } else {
            m_isDeviceManifestPresent = true;
        }

        QTimer::singleShot(0, this, &AssetsDownloadOperation::advanceOperationState);
    });
}

void AssetsDownloadOperation::readDeviceManifest()
{
    auto *buf = new QBuffer(this);

    if(!buf->open(QIODevice::ReadWrite)) {
        return finishWithError(buf->errorString());
    }

    auto *operation = cli()->read(QByteArrayLiteral("/ext/Manifest"), buf);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(!operation->isError()) {
            m_deviceManifest = AssetManifest(buf->readAll());
        }

        QTimer::singleShot(0, this, &AssetsDownloadOperation::advanceOperationState);
    });
}

void AssetsDownloadOperation::buildFileLists()
{
    FileNode::FileInfoList deleted, added, changed;

    FileNode::FileInfo manifestInfo;
    manifestInfo.name = QStringLiteral("Manifest");
    manifestInfo.absolutePath = manifestInfo.name;
    manifestInfo.type = FileNode::Type::RegularFile;

    print_file_list("<<<<< Local manifest:", m_localManifest.tree()->toPreOrderList());

    if(!m_isDeviceManifestPresent || m_deviceManifest.isError()) {
        info_msg("Device manifest not present or corrupt, assumimg fresh install...");

        changed.append(manifestInfo);
        added.append(m_localManifest.tree()->toPreOrderList().mid(1));

        std::copy_if(added.cbegin(), added.cend(), std::back_inserter(deleted), [](const FileNode::FileInfo &arg) {
            return arg.type == FileNode::Type::RegularFile;
        });

    } else {
        print_file_list(">>>>> Device manifest:", m_deviceManifest.tree()->toPreOrderList());

        deleted.append(m_localManifest.tree()->difference(m_deviceManifest.tree()));
        added.append(m_deviceManifest.tree()->difference(m_localManifest.tree()));
        changed.append(m_deviceManifest.tree()->changed(m_localManifest.tree()));

        std::remove_if(deleted.begin(), deleted.end(), [](const FileNode::FileInfo &arg) {
            return arg.type != FileNode::Type::RegularFile;
        });

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

    m_deleteList.append(deleted);
    m_deleteList.append(changed);

    m_writeList.append(added);
    m_writeList.append(changed);

    // Start deleting by the farthest nodes
    std::reverse(m_deleteList.begin(), m_deleteList.end());

    QTimer::singleShot(0, this, &AssetsDownloadOperation::advanceOperationState);
}

void AssetsDownloadOperation::deleteFiles()
{
    if(m_deleteList.isEmpty()) {
        info_msg("No files to delete, skipping to write");
        QTimer::singleShot(0, this, &AssetsDownloadOperation::advanceOperationState);
    }

    deviceState()->setStatusString(tr("Deleting unneeded files..."));

    int numFiles = m_deleteList.size();

    for(const auto &fileInfo : qAsConst(m_deleteList)) {
        const auto isLastFile = (--numFiles == 0);
        const auto fileName = QByteArrayLiteral("/ext/") + fileInfo.absolutePath.toLocal8Bit();

        auto *operation = cli()->remove(fileName);

        connect(operation, &AbstractOperation::finished, this, [=]() {
            if(operation->isError()) {
                finishWithError(operation->errorString());
            } else if(isLastFile) {
                QTimer::singleShot(0, this, &AssetsDownloadOperation::advanceOperationState);
            }
        });
    }
}

void AssetsDownloadOperation::writeFiles()
{
    if(m_writeList.isEmpty()) {
        info_msg("No files to write, skipping to the end");
        QTimer::singleShot(0, this, &AssetsDownloadOperation::advanceOperationState);
    }

    deviceState()->setStatusString(tr("Writing new files..."));

    int i = m_writeList.size();

    for(const auto &fileInfo : qAsConst(m_writeList)) {
        --i;

        AbstractOperation *op;
        const auto filePath = QByteArrayLiteral("/ext/") + fileInfo.absolutePath.toLocal8Bit();

        if(fileInfo.type == FileNode::Type::Directory) {
            op = cli()->mkdir(filePath);

        } else if(fileInfo.type == FileNode::Type::RegularFile) {
            auto *buf = new QBuffer(this);
            if(!buf->open(QIODevice::ReadWrite)) {
                return finishWithError(buf->errorString());
            }

            const auto resourcePath = QStringLiteral("resources/") + fileInfo.absolutePath;
            if((buf->write(m_archive.fileData(resourcePath)) <= 0) || (!buf->seek(0))) {
                return finishWithError(buf->errorString());
            }

            op = cli()->write(filePath, buf);

        } else {
            return finishWithError(QStringLiteral("Unexpected file type"));
        }

        connect(op, &AbstractOperation::finished, this, [=]() {
            if(op->isError()) {
                finishWithError(op->errorString());
            } else if(i == 0) {
                QTimer::singleShot(0, this, &AssetsDownloadOperation::advanceOperationState);
            }
        });
    }
}

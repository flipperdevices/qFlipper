#include "assetsdownloadoperation.h"

#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QBuffer>
#include <QFileInfo>
#include <QLoggingCategory>

#include "flipperzero/cli/removeoperation.h"
#include "flipperzero/cli/mkdiroperation.h"
#include "flipperzero/cli/storagewriteoperation.h"
#include "flipperzero/cli/storagereadoperation.h"
#include "flipperzero/cli/storageinfooperation.h"
#include "flipperzero/cli/storagestatoperation.h"

#include "flipperzero/commandinterface.h"
#include "flipperzero/assetmanifest.h"
#include "flipperzero/devicestate.h"

#include "gzipuncompressor.h"
#include "tempdirectories.h"
#include "tararchive.h"

Q_LOGGING_CATEGORY(CATEGORY_ASSETS, "ASSETS")

#define RESOURCES_PREFIX QByteArrayLiteral("resources")
#define DEVICE_MANIFEST QByteArrayLiteral("/ext/Manifest")

using namespace Flipper;
using namespace Zero;

static void printFileList(const char *header, const FileNode::FileInfoList &list)
{
    qCDebug(CATEGORY_ASSETS).noquote() << header;

    for(const auto &e : list) {
        const auto icon = QStringLiteral("[%1]").arg(e.type == FileNode::Type::RegularFile ? 'F' :
                                                     e.type == FileNode::Type::Directory   ? 'D' : '?');
        qCDebug(CATEGORY_ASSETS).noquote() << icon << e.absolutePath;
    }
}

AssetsDownloadOperation::AssetsDownloadOperation(CommandInterface *cli, DeviceState *deviceState, QIODevice *compressedFile, QObject *parent):
    AbstractUtilityOperation(cli, deviceState, parent),
    m_compressedFile(compressedFile),
    m_uncompressedFile(new QFile(globalTempDirs->root().absoluteFilePath(QStringLiteral("qFlipper-databases.tar")), this)),
    m_isDeviceManifestPresent(false)
{}

AssetsDownloadOperation::~AssetsDownloadOperation()
{}

const QString AssetsDownloadOperation::description() const
{
    return QStringLiteral("Assets Download @%1").arg(deviceState()->name());
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
        cleanup();
        finish();
    }
}

void AssetsDownloadOperation::checkForExtStorage()
{
    auto *op = cli()->storageInfo(QByteArrayLiteral("/ext"));

    connect(op, &AbstractOperation::finished, this, [=]() {
        if(op->isError()) {
            finishWithError("Failed to perform stat operation");
        } else if(!op->isPresent()) {
            qCDebug(CATEGORY_ASSETS) << "No external storage found, finishing early.";
            finish();
        } else {
            qCDebug(CATEGORY_ASSETS) << "External storage is present," << op->sizeFree() << "bytes free.";
            advanceOperationState();
        }
    });
}

void AssetsDownloadOperation::extractArchive()
{
    auto *uncompressor = new GZipUncompressor(m_compressedFile, m_uncompressedFile, this);

    if(uncompressor->isError()) {
        finishWithError(uncompressor->errorString());
        return;
    }

    connect(uncompressor, &GZipUncompressor::finished, this, [=]() {
        if(uncompressor->isError()) {
            finishWithError(uncompressor->errorString());
        } else {
            m_archive = std::move(TarArchive(m_uncompressedFile));

            if(m_archive.isError()) {
                finishWithError(m_archive.errorString());
            } else {
                QTimer::singleShot(0, this, &AssetsDownloadOperation::advanceOperationState);
            }
        }

        uncompressor->deleteLater();
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
    auto *operation = cli()->storageStat(DEVICE_MANIFEST);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            return finishWithError(operation->errorString());
        } else if(operation->type() != StorageStatOperation::Type::RegularFile) {
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

    auto *operation = cli()->storageRead(QByteArrayLiteral("/ext/Manifest"), buf);

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

    printFileList("<<<<< Local manifest:", m_localManifest.tree()->toPreOrderList());

    if(!m_isDeviceManifestPresent || m_deviceManifest.isError()) {
        qCDebug(CATEGORY_ASSETS) << "Device manifest not present or corrupt, assumimg fresh install...";

        changed.append(manifestInfo);
        added.append(m_localManifest.tree()->toPreOrderList().mid(1));

        std::copy_if(added.cbegin(), added.cend(), std::back_inserter(deleted), [](const FileNode::FileInfo &arg) {
            return arg.type == FileNode::Type::RegularFile;
        });

    } else {
        printFileList(">>>>> Device manifest:", m_deviceManifest.tree()->toPreOrderList());

        deleted.append(m_localManifest.tree()->difference(m_deviceManifest.tree()));
        added.append(m_deviceManifest.tree()->difference(m_localManifest.tree()));
        changed.append(m_deviceManifest.tree()->changed(m_localManifest.tree()));

        deleted.erase(std::remove_if(deleted.begin(), deleted.end(), [](const FileNode::FileInfo &arg) {
            return arg.type != FileNode::Type::RegularFile;
        }), deleted.end());

        if(!deleted.isEmpty() || !added.isEmpty() || !changed.isEmpty()) {
            changed.prepend(manifestInfo);
        }
    }

    if(!deleted.isEmpty()) {
        printFileList("----- Files deleted:", deleted);
    }

    if(!added.isEmpty()) {
        printFileList("+++++ Files added:", added);
    }

    if(!changed.isEmpty()) {
        printFileList("***** Files changed:", changed);
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
        qCDebug(CATEGORY_ASSETS) << "No files to delete, skipping to write";
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
        qCDebug(CATEGORY_ASSETS) << "No files to write, skipping to the end";
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

            op = cli()->storageWrite(filePath, buf);

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

void AssetsDownloadOperation::cleanup()
{
    m_uncompressedFile->remove();
}

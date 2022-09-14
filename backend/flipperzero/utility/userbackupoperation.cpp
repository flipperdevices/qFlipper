#include "userbackupoperation.h"

#include <QUrl>
#include <QFile>
#include <QDebug>

#include "flipperzero/devicestate.h"
#include "flipperzero/protobufsession.h"
#include "flipperzero/rpc/storagereadoperation.h"

#include "getfiletreeoperation.h"
#include "tarzipcompressor.h"
#include "tempdirectories.h"

using namespace Flipper;
using namespace Zero;

UserBackupOperation::UserBackupOperation(ProtobufSession *rpc, DeviceState *deviceState, const QUrl &backupUrl, QObject *parent):
    AbstractUtilityOperation(rpc, deviceState, parent),
    m_backupUrl(backupUrl),
    m_tempDir(QStringLiteral("%1/%2-backup-XXXXXX").arg(globalTempDirs->root().absolutePath(), deviceState->deviceInfo().name)),
    m_workDir(m_tempDir.path()),
    m_deviceDirName(QByteArrayLiteral("/int"))
{
    m_workDir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);
    m_workDir.setSorting(QDir::Name | QDir::DirsFirst);
}

const QString UserBackupOperation::description() const
{
    return QStringLiteral("Backup %1 @%2").arg(m_deviceDirName, deviceState()->name());
}

void UserBackupOperation::nextStateLogic()
{
    if(operationState() == Ready) {
        deviceState()->setStatusString(QStringLiteral("Backing up internal storage..."));
        setOperationState(CreatingDirectory);
        createDirectory();

    } else if(operationState() == CreatingDirectory) {
        setOperationState(GettingFileTree);
        getFileTree();

    } else if(operationState() == GettingFileTree) {
        setOperationState(ReadingFiles);
        readFiles();

    } else if(operationState() == ReadingFiles) {
        setOperationState(CreatingArchive);
        createArchive();

    } else if(operationState() == CreatingArchive) {
        finish();
    }
}

void UserBackupOperation::createDirectory()
{
    if(!m_deviceDirName.startsWith('/')) {
        finishWithError(BackendError::UnknownError, QStringLiteral("Expecting absolute path for device directory"));
    } else if(!m_workDir.mkpath(m_deviceDirName.mid(1))) {
        finishWithError(BackendError::DiskError, QStringLiteral("Failed to create backup directory"));
    } else {
        advanceOperationState();
    }
}

void UserBackupOperation::getFileTree()
{
    auto *operation = new GetFileTreeOperation(rpc(), deviceState(), m_deviceDirName, this);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(BackendError::BackupError, operation->errorString());
        } else {
            m_fileList = operation->files();
            advanceOperationState();
        }

        operation->deleteLater();
    });

    operation->start();
}

void UserBackupOperation::readFiles()
{
    auto numFiles = std::count_if(m_fileList.cbegin(), m_fileList.cend(), [](const FileInfo &arg) {
        return arg.type == FileType::RegularFile;
    });

    for(const auto &fileInfo: qAsConst(m_fileList)) {
        const auto filePath = fileInfo.absolutePath.mid(1);

        if(fileInfo.type == FileType::Directory) {
            if(!m_workDir.mkdir(filePath)) {
                finishWithError(BackendError::DiskError, QStringLiteral("Failed to create directory: %1").arg(QString(filePath)));
                return;
            }

        } else if(fileInfo.type == FileType::RegularFile) {
            const auto isLastFile = (--numFiles == 0);

            auto *file = new QFile(m_workDir.absoluteFilePath(filePath), this);
            auto *op = rpc()->storageRead(fileInfo.absolutePath, file);

            connect(op, &AbstractOperation::finished, this, [=]() {
                if(op->isError()) {
                    finishWithError(BackendError::BackupError, op->errorString());
                } else if(isLastFile) {
                    advanceOperationState();
                }
            });
        }
    }
}

void UserBackupOperation::createArchive()
{
    auto *compressor = new TarZipCompressor(m_workDir, m_backupUrl, this);

    connect(compressor, &TarZipCompressor::finished, this, [=]() {
        if(compressor->isError()) {
            finishWithError(compressor->error(), compressor->errorString());
        } else {
            advanceOperationState();
        }
    });
}

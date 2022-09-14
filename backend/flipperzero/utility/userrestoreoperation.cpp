#include "userrestoreoperation.h"

#include <QFile>
#include <QDirIterator>

#include "flipperzero/devicestate.h"
#include "flipperzero/protobufsession.h"
#include "flipperzero/rpc/storagemkdiroperation.h"
#include "flipperzero/rpc/storagewriteoperation.h"
#include "flipperzero/rpc/storageremoveoperation.h"

#include "tarzipuncompressor.h"
#include "tempdirectories.h"

using namespace Flipper;
using namespace Zero;

UserRestoreOperation::UserRestoreOperation(ProtobufSession *rpc, DeviceState *deviceState, const QUrl &backupUrl, QObject *parent):
    AbstractUtilityOperation(rpc, deviceState, parent),
    m_backupUrl(backupUrl),
    m_tempDir(QStringLiteral("%1/%2-backup-XXXXXX").arg(globalTempDirs->root().absolutePath(), deviceState->deviceInfo().name)),
    m_workDir(m_tempDir.path()),
    m_remoteDirName(QByteArrayLiteral("/int"))
{
    m_workDir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);
    m_workDir.setSorting(QDir::Name | QDir::DirsFirst);
}

const QString UserRestoreOperation::description() const
{
    return QStringLiteral("Restore %1 @%2").arg(m_remoteDirName, deviceState()->name());
}

void UserRestoreOperation::nextStateLogic()
{
    if(operationState() == Ready) {
        setOperationState(UncompressingArchive);
        uncompressArchive();

    } else if(operationState() == UncompressingArchive) {
        setOperationState(State::ReadingBackupDir);
        readBackupDir();

    } else if(operationState() == State::ReadingBackupDir) {
        setOperationState(State::DeletingFiles);
        deleteFiles();

    } else if(operationState() == State::DeletingFiles) {
        setOperationState(State::WritingFiles);
        writeFiles();

    } else if(operationState() == State::WritingFiles) {
        finish();
    }
}

void UserRestoreOperation::uncompressArchive()
{
    auto *tarZipFile = new QFile(m_backupUrl.toLocalFile(), this);
    auto *uncompressor = new TarZipUncompressor(tarZipFile, m_workDir, this);

    if(uncompressor->isError()) {
        finishWithError(uncompressor->error(), uncompressor->errorString());
        return;
    }

    connect(uncompressor, &TarZipUncompressor::finished, this, [=]() {
        if(uncompressor->isError()) {
            finishWithError(uncompressor->error(), uncompressor->errorString());
        } else {
            advanceOperationState();
        }
    });
}

void UserRestoreOperation::readBackupDir()
{
    if(!m_workDir.exists(m_remoteDirName.mid(1))) {
        finishWithError(BackendError::DiskError, QStringLiteral("No matching backup directory"));
        return;
    }

    QDirIterator it(m_workDir, QDirIterator::Subdirectories);

    while(it.hasNext()) {
        m_files.append(QFileInfo(it.next()));
    }

    if(m_files.isEmpty()) {
        finishWithError(BackendError::DiskError, QStringLiteral("Backup directory is empty"));
    } else {
        advanceOperationState();
    }
}

void UserRestoreOperation::deleteFiles()
{
    deviceState()->setStatusString(tr("Cleaning up..."));

    auto numFiles = m_files.size();
    for(const auto &fileInfo : qAsConst(m_files)) {
        const auto filePath = QByteArrayLiteral("/") + m_workDir.relativeFilePath(fileInfo.absoluteFilePath()).toLocal8Bit();
        const auto isLastFile = (--numFiles == 0);

        auto *op = rpc()->storageRemove(filePath);
        connect(op, &AbstractOperation::finished, this, [=]() {
            if(op->isError()) {
                finishWithError(BackendError::OperationError, op->errorString());
            } else if(isLastFile) {
                advanceOperationState();
            }
        });
    }
}

void UserRestoreOperation::writeFiles()
{
    deviceState()->setStatusString(tr("Restoring backup..."));

    auto numFiles = m_files.size();

    for(const auto &fileInfo: qAsConst(m_files)) {
        const auto filePath = QByteArrayLiteral("/") + m_workDir.relativeFilePath(fileInfo.absoluteFilePath()).toLocal8Bit();
        const auto isLastFile = (--numFiles == 0);

        AbstractOperation *op;

        if(fileInfo.isFile()) {
            auto *file = new QFile(fileInfo.absoluteFilePath(), this);

            op = rpc()->storageWrite(filePath, file);
            connect(op, &AbstractOperation::finished, this, [=]() {
                file->deleteLater();
            });

        } else if(fileInfo.isDir()) {
            op = rpc()->storageMkdir(filePath);
        } else {
            finishWithError(BackendError::UnknownError, QStringLiteral("Expected a file or a directory"));
            return;
        }

        connect(op, &AbstractOperation::finished, this, [=]() {
            if(op->isError()) {
                finishWithError(BackendError::OperationError, op->errorString());
            } else if(isLastFile) {
                advanceOperationState();
            }

            op->deleteLater();
        });
    }
}

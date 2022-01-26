#include "userrestoreoperation.h"

#include <QFile>
#include <QTimer>
#include <QDirIterator>

#include "flipperzero/devicestate.h"
#include "flipperzero/commandinterface.h"
#include "flipperzero/rpc/storagemkdiroperation.h"
#include "flipperzero/rpc/storagewriteoperation.h"
#include "flipperzero/rpc/storageremoveoperation.h"

#include "debug.h"

using namespace Flipper;
using namespace Zero;

UserRestoreOperation::UserRestoreOperation(CommandInterface *cli, DeviceState *deviceState, const QString &backupPath, QObject *parent):
    AbstractUtilityOperation(cli, deviceState, parent),
    m_backupDir(backupPath),
    m_deviceDirName(QByteArrayLiteral("/int"))
{
    m_backupDir.setFilter(QDir::Dirs  | QDir::Files | QDir::NoDotAndDotDot);
    m_backupDir.setSorting(QDir::Name | QDir::DirsFirst);
}

const QString UserRestoreOperation::description() const
{
    return QStringLiteral("Restore %1 @%2").arg(m_deviceDirName, deviceState()->name());
}

void UserRestoreOperation::nextStateLogic()
{
    if(operationState() == BasicOperationState::Ready) {
        setOperationState(State::ReadingBackupDir);
        if(!readBackupDir()) {
            finishWithError(BackendError::DiskError, QStringLiteral("Failed to process backup directory"));
        } else {
            QTimer::singleShot(0, this, &UserRestoreOperation::nextStateLogic);
        }

    } else if(operationState() == State::ReadingBackupDir) {
        setOperationState(State::DeletingFiles);
        if(!deleteFiles()) {
            finishWithError(BackendError::DiskError, QStringLiteral("Failed to delete old files"));
        }

    } else if(operationState() == State::DeletingFiles) {
        setOperationState(State::WritingFiles);
        if(!writeFiles()) {
            finishWithError(BackendError::DiskError, QStringLiteral("Failed to write new files"));
        }

    } else if(operationState() == State::WritingFiles) {
        finish();
    } else {}
}

bool UserRestoreOperation::readBackupDir()
{
    const auto &name = deviceState()->deviceInfo().name;
    const auto subdir = name + m_deviceDirName;

    check_return_bool(m_backupDir.exists(subdir), "Requested directory not found");
    check_return_bool(m_backupDir.cd(subdir), "Access denied");

    QDirIterator it(m_backupDir, QDirIterator::Subdirectories);

    while(it.hasNext()) {
        it.next();
        m_files.append(it.fileInfo());
    }

    check_return_bool(!m_files.isEmpty(), "Backup directory is empty.");
    return true;
}

bool UserRestoreOperation::deleteFiles()
{
    deviceState()->setStatusString(tr("Cleaning up..."));

    auto numFiles = m_files.size();
    for(auto it = m_files.crbegin(); it != m_files.crend(); ++it) {
        check_return_bool(it->isFile() || it->isDir(), "Expected a file or directory");

        const auto filePath = m_deviceDirName + QByteArrayLiteral("/") + m_backupDir.relativeFilePath(it->absoluteFilePath()).toLocal8Bit();
        const auto isLastFile = (--numFiles == 0);

        auto *op = rpc()->storageRemove(filePath);
        connect(op, &AbstractOperation::finished, this, [=](){
            if(op->isError()) {
                finishWithError(op->error(), op->errorString());
            } else if(isLastFile) {
                QTimer::singleShot(0, this, &UserRestoreOperation::nextStateLogic);
            }
        });
    }

    return true;
}

bool UserRestoreOperation::writeFiles()
{
    deviceState()->setStatusString(tr("Restoring backup..."));

    auto numFiles = m_files.size();

    for(const auto &fileInfo: qAsConst(m_files)) {
        const auto filePath = m_deviceDirName + QByteArrayLiteral("/") + m_backupDir.relativeFilePath(fileInfo.absoluteFilePath()).toLocal8Bit();
        const auto isLastFile = (--numFiles == 0);

        AbstractOperation *op;

        if(fileInfo.isFile()) {
            auto *file = new QFile(fileInfo.absoluteFilePath(), this);

            if(!file->open(QIODevice::ReadOnly)) {
                file->deleteLater();
                error_msg(QStringLiteral("Failed to open file for reading: %1.").arg(file->errorString()));
                return false;
            }

            op = rpc()->storageWrite(filePath, file);
            connect(op, &AbstractOperation::finished, this, [=]() {
                file->close();
                file->deleteLater();
            });

        } else if(fileInfo.isDir()) {
            op = rpc()->storageMkdir(filePath);
        } else {
            return false;
        }

        connect(op, &AbstractOperation::finished, this, [=]() {
            if(op->isError()) {
                finishWithError(op->error(), op->errorString());
            } else if(isLastFile) {
                QTimer::singleShot(0, this, &UserRestoreOperation::nextStateLogic);
            }

            op->deleteLater();
        });
    }

    return true;
}

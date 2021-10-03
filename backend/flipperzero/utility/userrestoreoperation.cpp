#include "userrestoreoperation.h"

#include <QUrl>
#include <QFile>
#include <QTimer>
#include <QDirIterator>

#include "flipperzero/flipperzero.h"
#include "flipperzero/commandinterface.h"
#include "flipperzero/cli/mkdiroperation.h"
#include "flipperzero/cli/writeoperation.h"
#include "flipperzero/cli/removeoperation.h"

#include "macros.h"

using namespace Flipper;
using namespace Zero;

UserRestoreOperation::UserRestoreOperation(FlipperZero *device, const QString &backupPath, QObject *parent):
    FlipperZeroOperation(device, parent),
    m_backupDir(QUrl(backupPath).toLocalFile()),
    m_deviceDirName(QByteArrayLiteral("/int"))
{
    m_backupDir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    m_backupDir.setSorting(QDir::Name | QDir::DirsFirst);
}

const QString UserRestoreOperation::description() const
{
    return QStringLiteral("Restore user data @%1 %2").arg(device()->model(), device()->name());
}

void UserRestoreOperation::transitionToNextState()
{
    if(operationState() == BasicState::Ready) {
        setOperationState(State::ReadingBackupDir);
        if(!readBackupDir()) {
            finishWithError(QStringLiteral("Failed to process backup directory"));
        } else {
            QTimer::singleShot(0, this, &UserRestoreOperation::transitionToNextState);
        }

    } else if(operationState() == State::ReadingBackupDir) {
        setOperationState(State::DeletingFiles);
        if(!deleteFiles()) {
            finishWithError(QStringLiteral("Failed to delete old files"));
        }

    } else if(operationState() == State::DeletingFiles) {
        setOperationState(State::WritingFiles);
        if(!writeFiles()) {
            finishWithError(QStringLiteral("Failed to write new files"));
        }

    } else if(operationState() == State::WritingFiles) {
        finish();
    } else {}
}

bool UserRestoreOperation::readBackupDir()
{
    const auto subdir = device()->name() + m_deviceDirName;

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
    device()->setMessage(QStringLiteral("Cleaning up..."));

    auto numFiles = m_files.size();
    for(auto it = m_files.crbegin(); it != m_files.crend(); ++it) {
        check_return_bool(it->isFile() || it->isDir(), "Expected a file or directory");

        const auto filePath = m_deviceDirName + QByteArrayLiteral("/") + m_backupDir.relativeFilePath(it->absoluteFilePath()).toLocal8Bit();
        const auto isLastFile = (--numFiles == 0);

        auto *op = device()->cli()->remove(filePath);
        connect(op, &AbstractOperation::finished, this, [=](){
            if(op->isError()) {
                finishWithError(op->errorString());
            } else if(isLastFile) {
                QTimer::singleShot(0, this, &UserRestoreOperation::transitionToNextState);
            }

            op->deleteLater();
        });
    }

    return true;
}

bool UserRestoreOperation::writeFiles()
{
    device()->setMessage(QStringLiteral("Restoring backup..."));

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

            op = device()->cli()->write(filePath, file);
            connect(op, &AbstractOperation::finished, this, [=]() {
                file->close();
                file->deleteLater();
            });

        } else if(fileInfo.isDir()) {
            op = device()->cli()->mkdir(filePath);
        } else {
            return false;
        }

        connect(op, &AbstractOperation::finished, this, [=]() {
            if(op->isError()) {
                finishWithError(op->errorString());
            } else if(isLastFile) {
                QTimer::singleShot(0, this, &UserRestoreOperation::transitionToNextState);
            }

            op->deleteLater();
        });
    }

    return true;
}

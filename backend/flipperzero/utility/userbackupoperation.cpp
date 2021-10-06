#include "userbackupoperation.h"

#include <QUrl>
#include <QFile>
#include <QTimer>

#include "flipperzero/devicestate.h"
#include "flipperzero/commandinterface.h"
#include "flipperzero/cli/readoperation.h"

#include "getfiletreeoperation.h"
#include "macros.h"

#define CALL_LATER(obj, func) (QTimer::singleShot(0, obj, func))

using namespace Flipper;
using namespace Zero;

UserBackupOperation::UserBackupOperation(CommandInterface *cli, DeviceState *deviceState, const QString &backupPath, QObject *parent):
    AbstractUtilityOperation(cli, deviceState, parent),
    m_backupDir(backupPath),
    m_deviceDirName(QByteArrayLiteral("/int"))
{}

const QString UserBackupOperation::description() const
{
    const auto &model = deviceState()->deviceInfo().model;
    const auto &name = deviceState()->deviceInfo().name;

    return QStringLiteral("Backup user data @%1 %2").arg(model, name);
}

void UserBackupOperation::advanceOperationState()
{
    if(operationState() == BasicOperationState::Ready) {
        setOperationState(State::CreatingDirectory);

        deviceState()->setStatusString(QStringLiteral("Backing up internal storage..."));

        if(!m_deviceDirName.startsWith('/')) {
            finishWithError(QStringLiteral("Expecting absolute path for device directory"));
        } else if(!createBackupDirectory()) {
            finishWithError(QStringLiteral("Failed to create backup directory"));
        } else {
            CALL_LATER(this, &UserBackupOperation::advanceOperationState);
        }

    } else if(operationState() == State::CreatingDirectory) {
        setOperationState(State::GettingFileTree);

        auto *op = new GetFileTreeOperation(cli(), deviceState(), m_deviceDirName, this);

        connect(op, &AbstractOperation::finished, this, [=]() {
            if(op->isError()) {
                finishWithError(op->errorString());
            } else {
                m_fileList = op->result();
                CALL_LATER(this, &UserBackupOperation::advanceOperationState);
            }

            op->deleteLater();
        });

        op->start();

    } else if(operationState() == State::GettingFileTree) {
        setOperationState(State::ReadingFiles);

        if(!readFiles()) {
            finishWithError(QStringLiteral("Failed to read files from device"));
        }
    }
}

bool UserBackupOperation::createBackupDirectory()
{
    const auto &subdir = deviceState()->deviceInfo().name;
    const QFileInfo targetDirInfo(m_backupDir, subdir);

    if(targetDirInfo.isDir()) {
        QDir d(targetDirInfo.absoluteFilePath());

        if(!d.removeRecursively()) {
            return false;
        }

    } else if(targetDirInfo.exists()) {
        return false;
    }

    return m_backupDir.mkpath(subdir + m_deviceDirName) && m_backupDir.cd(subdir);
}

bool UserBackupOperation::readFiles()
{
    auto numFiles = std::count_if(m_fileList.cbegin(), m_fileList.cend(), [](const FileInfo &arg) {
        return arg.type == FileType::RegularFile;
    });

    for(const auto &fileInfo: qAsConst(m_fileList)) {
        const auto filePath = fileInfo.absolutePath.mid(1);

        if(fileInfo.type == FileType::Directory) {
            if(!m_backupDir.mkdir(filePath)) {
                return false;
            }

        } else if(fileInfo.type == FileType::RegularFile) {
            const auto isLastFile = (--numFiles == 0);

            auto *file = new QFile(m_backupDir.absoluteFilePath(filePath), this);
            if(!file->open(QIODevice::WriteOnly)) {
                file->deleteLater();
                return false;
            }

            auto *op = cli()->read(fileInfo.absolutePath, file);
            connect(op, &AbstractOperation::finished, this, [=]() {
                if(op->isError()) {
                    finishWithError(op->errorString());
                } else if(isLastFile) {
                    finish();
                }

                file->close();
                file->deleteLater();
            });
        }
    }

    return true;
}

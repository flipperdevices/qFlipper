#include "userbackupoperation.h"

#include <QUrl>
#include <QFile>
#include <QTimer>

#include "flipperzero/storage/readoperation.h"

#include "flipperzero/storagecontroller.h"
#include "flipperzero/flipperzero.h"

#include "getfiletreeoperation.h"
#include "macros.h"

using namespace Flipper;
using namespace Zero;

UserBackupOperation::UserBackupOperation(FlipperZero *device, const QString &backupPath, QObject *parent):
    Operation(device, parent),
    m_backupDir(QUrl(backupPath).toLocalFile()),
    m_deviceDirName(QByteArrayLiteral("/int"))
{}

const QString UserBackupOperation::description() const
{
    return QStringLiteral("Backup user data @%1 %2").arg(device()->model(), device()->name());
}

void UserBackupOperation::transitionToNextState()
{
    if(state() == BasicState::Ready) {
        setState(State::CreatingDirectory);

        if(!m_deviceDirName.startsWith('/')) {
            finishWithError(QStringLiteral("Expecting absolute path for device directory"));
        } else if(!createBackupDirectory()) {
            finishWithError(QStringLiteral("Failed to create backup directory"));
        } else {
            QTimer::singleShot(0, this, &UserBackupOperation::transitionToNextState);
        }

    } else if(state() == State::CreatingDirectory) {
        setState(State::GettingFileTree);

        auto *op = new GetFileTreeOperation(device(), m_deviceDirName, this);

        connect(op, &AbstractOperation::finished, this, [=]() {
            if(op->isError()) {
                finishWithError(op->errorString());
            } else {
                m_fileList = op->result();
                QTimer::singleShot(0, this, &UserBackupOperation::transitionToNextState);
            }

            op->deleteLater();
        });

        op->start();

    } else if(state() == State::GettingFileTree) {
        setState(State::ReadingFiles);

        if(!readFiles()) {
            finishWithError(QStringLiteral("Failed to read files from device"));
        }
    }
}

bool UserBackupOperation::createBackupDirectory()
{
    const auto &subdir = device()->name();
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

            auto *op = device()->storage()->read(fileInfo.absolutePath, file);
            connect(op, &AbstractOperation::finished, this, [=]() {
                if(op->isError()) {
                    finishWithError(op->errorString());
                }

                op->deleteLater();

                file->close();
                file->deleteLater();

                if(isLastFile) {
                    finish();
                }
            });
        }
    }

    return true;
}

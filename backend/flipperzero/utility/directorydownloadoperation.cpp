#include "directorydownloadoperation.h"

#include <QUrl>
#include <QFile>
#include <QDebug>

#include "flipperzero/devicestate.h"
#include "flipperzero/protobufsession.h"
#include "flipperzero/rpc/storagereadoperation.h"

#include "getfiletreeoperation.h"

using namespace Flipper;

using namespace Zero;

DirectoryDownloadOperation::DirectoryDownloadOperation(ProtobufSession *rpc, DeviceState *deviceState,
                                                       const QString &targetPath, const QByteArray &remotePath, QObject *parent):
    AbstractUtilityOperation(rpc, deviceState, parent),
    m_targetDir(targetPath),
    m_remotePath(remotePath),
    m_totalSize(0)
{}

const QString DirectoryDownloadOperation::description() const
{
    return QStringLiteral("Download Directory %1").arg(QString(m_remotePath));
}

void DirectoryDownloadOperation::nextStateLogic()
{
    if(operationState() == BasicOperationState::Ready) {
        setOperationState(State::CreatingDirectory);
        createLocalDirectory();

    } else if(operationState() == State::CreatingDirectory) {
        setOperationState(State::GettingFileTree);
        getFileTree();

    } else if(operationState() == State::GettingFileTree) {
        setOperationState(State::ReadingFiles);
        readFiles();

    } else if(operationState() == State::ReadingFiles) {
        finish();
    }
}

void DirectoryDownloadOperation::createLocalDirectory()
{
    const auto subdir = QUrl(m_remotePath).fileName();

    if(m_targetDir.exists(subdir)) {
        const auto success = m_targetDir.cd(subdir) && m_targetDir.removeRecursively() && m_targetDir.cdUp();
        if(!success) {
            finishWithError(BackendError::DiskError, QStringLiteral("Target directory exists, but cannot be removed"));
            return;
        }
    }

    if(!(m_targetDir.mkdir(subdir) && m_targetDir.cd(subdir))) {
        finishWithError(BackendError::DiskError, QStringLiteral("Failed to create directory"));
    } else {
        advanceOperationState();
    }
}

void DirectoryDownloadOperation::getFileTree()
{
    auto *operation = new GetFileTreeOperation(rpc(), deviceState(), m_remotePath, this);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(BackendError::BackupError, operation->errorString());
        } else {
            m_fileList = operation->files();
            m_totalSize = std::accumulate(m_fileList.cbegin(), m_fileList.cend(), 0, [](qint64 total, const FileInfo &arg) {
                return arg.type == FileType::RegularFile ? total + arg.size : total;
            });

            advanceOperationState();
        }

        operation->deleteLater();
    });

    operation->start();
}

void DirectoryDownloadOperation::readFiles()
{
    auto filesRemaining = std::count_if(m_fileList.cbegin(), m_fileList.cend(), [](const FileInfo &arg) {
        return arg.type == FileType::RegularFile;
    });

    auto baseProgress = 0.0;

    for(const auto &fileInfo: qAsConst(m_fileList)) {
        const auto filePath = fileInfo.absolutePath.mid(m_remotePath.size() + 1);

        if(fileInfo.type == FileType::Directory) {
            if(!m_targetDir.mkdir(filePath)) {
                finishWithError(BackendError::DiskError, QStringLiteral("Failed to create directory: %1").arg(QString(filePath)));
                return;
            }

        } else if(fileInfo.type == FileType::RegularFile) {
            auto *file = new QFile(m_targetDir.absoluteFilePath(filePath), this);
            auto *operation = rpc()->storageRead(fileInfo.absolutePath, file);

            const auto isLast = (--filesRemaining == 0);

            connect(operation, &AbstractOperation::progressChanged, this, [=]() {
                setProgress(baseProgress + operation->progress() * fileInfo.size / m_totalSize);
            });

            connect(operation, &AbstractOperation::finished, this, [=]() {
                if(operation->isError()) {
                    finishWithError(BackendError::BackupError, operation->errorString());
                } else if(isLast) {
                    advanceOperationState();
                }
            });

            baseProgress += fileInfo.size * 100.0 / m_totalSize;
        }
    }
}

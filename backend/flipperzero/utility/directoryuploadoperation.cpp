#include "directoryuploadoperation.h"

#include <QDirIterator>

#include "flipperzero/devicestate.h"
#include "flipperzero/protobufsession.h"
#include "flipperzero/rpc/storagemkdiroperation.h"
#include "flipperzero/rpc/storagewriteoperation.h"

using namespace Flipper;
using namespace Zero;

DirectoryUploadOperation::DirectoryUploadOperation(ProtobufSession *rpc, DeviceState *deviceState, const QString &localDir,
                                                   const QByteArray &remotePath, QObject *parent):
    AbstractUtilityOperation(rpc, deviceState, parent),
    m_localDir(localDir),
    m_remotePath(remotePath),
    m_totalSize(0)
{
    m_localDir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);
}

const QString DirectoryUploadOperation::description() const
{
    return QStringLiteral("Upload Directory %1").arg(QString(m_localDir.dirName()));
}

void DirectoryUploadOperation::nextStateLogic()
{
    if(operationState() == Ready) {
        setOperationState(ReadingLocalDir);
        readLocalDir();

    } else if(operationState() == ReadingLocalDir) {
        setOperationState(CreatingRemoteDir);
        createRemoteDir();

    } else if(operationState() == CreatingRemoteDir) {
        setOperationState(WritingFiles);
        writeFiles();

    } else if(operationState() == WritingFiles) {
        finish();
    }
}

void DirectoryUploadOperation::readLocalDir()
{
    QDirIterator it(m_localDir, QDirIterator::Subdirectories);

    while(it.hasNext()) {
        it.next();
        const auto &fileInfo = it.fileInfo();
        if(fileInfo.isFile()) {
            m_totalSize += fileInfo.size();
        }
        m_files.append(fileInfo);
    }

    if(m_files.isEmpty()) {
        finish();
    } else {
        advanceOperationState();
    }
}

void DirectoryUploadOperation::createRemoteDir()
{
    const auto remoteDirPath = m_remotePath + QByteArrayLiteral("/") + m_localDir.dirName().toLocal8Bit();
    auto *operation = rpc()->storageMkdir(remoteDirPath);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(BackendError::OperationError, QStringLiteral("Failed to create directory: %1").arg(operation->errorString()));
        } else {
            advanceOperationState();
        }
    });
}

void DirectoryUploadOperation::writeFiles()
{
    auto numFiles = m_files.size();
    auto startProgress = 0.0;

    for(const auto &fileInfo: qAsConst(m_files)) {
        const auto relativeFilePath = m_localDir.relativeFilePath(fileInfo.absoluteFilePath()).toLocal8Bit();
        const auto remoteFilePath = m_remotePath + QByteArrayLiteral("/") + m_localDir.dirName().toLocal8Bit() + QByteArrayLiteral("/") + relativeFilePath;
        const auto isLastFile = !(--numFiles);

        AbstractOperation *operation;

        if(fileInfo.isDir()) {
            operation = rpc()->storageMkdir(remoteFilePath);
        } else {
            auto *file = new QFile(fileInfo.absoluteFilePath(), this);
            const auto sizeWeight = (double)fileInfo.size() / m_totalSize;

            operation = rpc()->storageWrite(remoteFilePath, file);
            connect(operation, &AbstractOperation::progressChanged, this, [=]() {
                setProgress(startProgress + operation->progress() * sizeWeight);
            });
            connect(operation, &AbstractOperation::finished, this, [=]() {
                file->deleteLater();
            });

            startProgress += sizeWeight * 100.0;
        }

        connect(operation, &AbstractOperation::finished, this, [=]() {
            if(operation->isError()) {
                finishWithError(BackendError::OperationError, operation->errorString());
            } else if(isLastFile) {
                advanceOperationState();
            }

            operation->deleteLater();
        });
    }
}

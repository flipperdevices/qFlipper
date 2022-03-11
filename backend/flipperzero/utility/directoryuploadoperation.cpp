#include "directoryuploadoperation.h"

#include <QDirIterator>

#include "flipperzero/protobufsession.h"
#include "flipperzero/rpc/storagemkdiroperation.h"
#include "flipperzero/rpc/storagewriteoperation.h"

using namespace Flipper;
using namespace Zero;

DirectoryUploadOperation::DirectoryUploadOperation(ProtobufSession *rpc, DeviceState *deviceState, const QString &localDir,
                                                   const QByteArray &remotePath, QObject *parent):
    AbstractUtilityOperation(rpc, deviceState, parent),
    m_localDir(localDir),
    m_remotePath(remotePath)
{}

const QString DirectoryUploadOperation::description() const
{
    return QStringLiteral("Upload Directory %1").arg(QString(m_remotePath));
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
        m_files.append(it.fileInfo());
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

    for(const auto &fileInfo: qAsConst(m_files)) {
        const auto relativeFilePath = m_localDir.relativeFilePath(fileInfo.absoluteFilePath()).toLocal8Bit();
        const auto remoteFilePath = m_remotePath + QByteArrayLiteral("/") + m_localDir.dirName().toLocal8Bit() + QByteArrayLiteral("/") + relativeFilePath;
        const auto isLastFile = !(--numFiles);

        AbstractOperation *operation;

        if(fileInfo.isDir()) {
            operation = rpc()->storageMkdir(remoteFilePath);
        } else {
            auto *file = new QFile(fileInfo.absoluteFilePath(), this);

//            if(!file->open(QIODevice::ReadOnly)) {
//                file->deleteLater();
//                finishWithError(BackendError::DiskError, QStringLiteral("Failed to open file for reading: %1").arg(file->errorString()));
//                return;
//            }

            operation = rpc()->storageWrite(remoteFilePath, file);
            connect(operation, &AbstractOperation::finished, this, [=]() {
                file->deleteLater();
            });
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

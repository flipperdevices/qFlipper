#include "updateprepareoperation.h"

#include "flipperzero/devicestate.h"
#include "flipperzero/protobufsession.h"
#include "flipperzero/rpc/storagestatoperation.h"
#include "flipperzero/rpc/storagemkdiroperation.h"

using namespace Flipper;
using namespace Zero;

UpdatePrepareOperation::UpdatePrepareOperation(ProtobufSession *rpc, DeviceState *deviceState, const QByteArray &updateDirName, const QByteArray &remotePath, QObject *parent):
    AbstractUtilityOperation(rpc, deviceState, parent),
    m_updateDirName(updateDirName),
    m_remotePath(remotePath),
    m_needsCompleteUpload(false)
{}

const QString UpdatePrepareOperation::description() const
{
    return QStringLiteral("Prepare Update @%1").arg(deviceState()->deviceInfo().name);
}

bool UpdatePrepareOperation::needsCompleteUpload() const
{
    return m_needsCompleteUpload;
}

void UpdatePrepareOperation::nextStateLogic()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(UpdatePrepareOperation::CheckingRemotePath);
        checkRemotePath();

    } else if(operationState() == UpdatePrepareOperation::CheckingRemotePath) {
        setOperationState(UpdatePrepareOperation::CreatingRemotePath);
        createRemotePath();

    } else if(operationState() == UpdatePrepareOperation::CreatingRemotePath) {
        setOperationState(UpdatePrepareOperation::CheckingUpdateDirectory);
        checkUpdateDirectory();

    } else if(operationState() == UpdatePrepareOperation::CheckingUpdateDirectory) {
        setOperationState(UpdatePrepareOperation::CreatingUpdateDirectory);
        createUpdateDirectory();

    } else if(operationState() == UpdatePrepareOperation::CreatingUpdateDirectory) {
        finish();
    }
}

void UpdatePrepareOperation::checkDirectory(const QByteArray &remotePath, OperationState altState)
{
    auto *operation = rpc()->storageStat(remotePath);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->error(), operation->errorString());
            return;

        } else if(operation->type() == StorageStatOperation::RegularFile) {
            finishWithError(BackendError::UnknownError, QStringLiteral("Remote path %1 is a regular file").arg(QString(remotePath)));
            return;

        } else if(operation->type() == StorageStatOperation::Directory) {
            setOperationState(altState);
        }

        advanceOperationState();
    });
}

void UpdatePrepareOperation::createDirectory(const QByteArray &remotePath, OperationState altState)
{
    auto *operation = rpc()->storageMkdir(remotePath);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->error(), operation->errorString());

        } else {
            setOperationState(altState);
            advanceOperationState();
        }
    });

    m_needsCompleteUpload = true;
}

void UpdatePrepareOperation::checkRemotePath()
{
    checkDirectory(m_remotePath, CreatingRemotePath);
}

void UpdatePrepareOperation::createRemotePath()
{
    createDirectory(m_remotePath, CheckingUpdateDirectory);
}

void UpdatePrepareOperation::checkUpdateDirectory()
{
    checkDirectory(m_remotePath + '/' + m_updateDirName, CreatingUpdateDirectory);
}

void UpdatePrepareOperation::createUpdateDirectory()
{
    createDirectory(m_remotePath + '/' + m_updateDirName, CreatingUpdateDirectory);
}

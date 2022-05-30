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
    m_updateDirExists(false)
{}

const QString UpdatePrepareOperation::description() const
{
    return QStringLiteral("Prepare Update @%1").arg(deviceState()->deviceInfo().name);
}

bool UpdatePrepareOperation::updateDirectoryExists() const
{
    return m_updateDirExists;
}

void UpdatePrepareOperation::nextStateLogic()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(UpdatePrepareOperation::CheckingRemoteDirectory);
        checkRemoteDirectory();

    } else if(operationState() == UpdatePrepareOperation::CheckingRemoteDirectory) {
        setOperationState(UpdatePrepareOperation::CreatingRemoteDirectory);
        createRemoteDirectory();

    } else if(operationState() == UpdatePrepareOperation::CreatingRemoteDirectory) {
        setOperationState(UpdatePrepareOperation::CheckingUpdateDirectory);
        checkUpdateDirectory();

    } else if(operationState() == UpdatePrepareOperation::CheckingUpdateDirectory) {
        finish();
    }
}

void UpdatePrepareOperation::checkRemoteDirectory()
{
    auto *operation = rpc()->storageStat(m_remotePath);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->error(), operation->errorString());
            return;

        } else if(operation->type() == StorageStatOperation::Directory) {
            setOperationState(UpdatePrepareOperation::CreatingRemoteDirectory);
        }

        advanceOperationState();
    });
}

void UpdatePrepareOperation::createRemoteDirectory()
{
    auto *operation = rpc()->storageMkdir(m_remotePath);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->error(), operation->errorString());
        } else {
            advanceOperationState();
        }
    });
}

void UpdatePrepareOperation::checkUpdateDirectory()
{
    auto *operation = rpc()->storageStat(m_remotePath + QByteArrayLiteral("/") + m_updateDirName);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->error(), operation->errorString());
        } else {
            m_updateDirExists = operation->type() == StorageStatOperation::Directory;
            advanceOperationState();
        }
    });
}

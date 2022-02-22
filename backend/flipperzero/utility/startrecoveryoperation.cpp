#include "startrecoveryoperation.h"

#include "flipperzero/devicestate.h"
#include "flipperzero/protobufsession.h"
#include "flipperzero/rpc/systemrebootoperation.h"

using namespace Flipper;
using namespace Zero;

StartRecoveryOperation::StartRecoveryOperation(ProtobufSession *rpc, DeviceState *deviceState, QObject *parent):
    AbstractUtilityOperation(rpc, deviceState, parent)
{
// Workaround for Windows taking too long to install the driver
#ifdef Q_OS_WINDOWS
    setTimeout(120000);
#endif
}

const QString StartRecoveryOperation::description() const
{
    return QStringLiteral("Start Recovery Mode @%1").arg(deviceState()->name());
}

void StartRecoveryOperation::nextStateLogic()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(StartRecoveryOperation::WaitingForRecovery);
        startRecoveryMode();

    } else if(operationState() == StartRecoveryOperation::WaitingForRecovery) {
        disconnect(deviceState(), &DeviceState::isOnlineChanged, this, &StartRecoveryOperation::onDeviceOnlineChanged);
        finish();

    } else {}
}

void StartRecoveryOperation::onOperationTimeout()
{
    finishWithError(BackendError::RecoveryAccessError, QStringLiteral("Failed to start recovery mode: operation timeout"));
}

void StartRecoveryOperation::onDeviceOnlineChanged()
{
    if(deviceState()->isOnline()) {
        advanceOperationState();
    } else {
        startTimeout();
    }
}

void StartRecoveryOperation::startRecoveryMode()
{
    if(deviceState()->isRecoveryMode()) {
        nextStateLogic();
        return;
    }

    deviceState()->setStatusString(QStringLiteral("Starting Recovery mode..."));

    connect(deviceState(), &DeviceState::isOnlineChanged, this, &StartRecoveryOperation::onDeviceOnlineChanged);

    auto *operation = rpc()->rebootToRecovery();

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->error(), operation->errorString());
        } else {
            startTimeout();
        }
    });
}

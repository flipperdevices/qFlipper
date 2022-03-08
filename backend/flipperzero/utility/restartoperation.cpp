#include "restartoperation.h"

#include "flipperzero/devicestate.h"
#include "flipperzero/protobufsession.h"
#include "flipperzero/rpc/systemrebootoperation.h"

using namespace Flipper;
using namespace Zero;

RestartOperation::RestartOperation(ProtobufSession *rpc, DeviceState *deviceState, QObject *parent):
    AbstractUtilityOperation(rpc, deviceState, parent)
{}

const QString RestartOperation::description() const
{
    return QStringLiteral("Restart device @%1").arg(deviceState()->name());
}

void RestartOperation::nextStateLogic()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(RestartOperation::WaitingForOSBoot);
        rebootDevice();

    } else if(operationState() == RestartOperation::WaitingForOSBoot) {
        disconnect(deviceState(), &DeviceState::isOnlineChanged, this, &RestartOperation::onDeviceOnlineChanged);
        finish();

    } else {}
}

void RestartOperation::onOperationTimeout()
{
    finishWithError(BackendError::UnknownError, QStringLiteral("Failed to restart: timeout exceeded"));
}

void RestartOperation::onDeviceOnlineChanged()
{
    if(deviceState()->isOnline()) {
        advanceOperationState();
    } else {
        startTimeout();
    }
}

void RestartOperation::rebootDevice()
{
    deviceState()->setProgress(-1);
    deviceState()->setStatusString(QStringLiteral("Restarting device..."));

    connect(deviceState(), &DeviceState::isOnlineChanged, this, &RestartOperation::onDeviceOnlineChanged);

    auto *operation = rpc()->rebootToOS();

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->error(), operation->errorString());
        }
    });

    startTimeout();
}

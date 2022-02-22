#include "factoryresetutiloperation.h"

#include "flipperzero/devicestate.h"
#include "flipperzero/protobufsession.h"
#include "flipperzero/rpc/systemfactoryresetoperation.h"

using namespace Flipper;
using namespace Zero;

FactoryResetUtilOperation::FactoryResetUtilOperation(ProtobufSession *rpc, DeviceState *deviceState, QObject *parent):
    AbstractUtilityOperation(rpc, deviceState, parent)
{}

const QString FactoryResetUtilOperation::description() const
{
    return QStringLiteral("Factory reset (Utility) @%1").arg(deviceState()->name());
}

void FactoryResetUtilOperation::nextStateLogic()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(FactoryResetUtilOperation::ResettingDevice);
        resetDevice();

    } else if(operationState() == FactoryResetUtilOperation::ResettingDevice) {
        finish();
    }
}

void FactoryResetUtilOperation::onDeviceOnlineChanged()
{
    if(deviceState()->isOnline()) {
        advanceOperationState();
    } else {
        startTimeout();
    }
}

void FactoryResetUtilOperation::resetDevice()
{
    deviceState()->setStatusString(QStringLiteral("Performing factory reset..."));

    connect(deviceState(), &DeviceState::isOnlineChanged, this, &FactoryResetUtilOperation::onDeviceOnlineChanged);

    auto *operation = rpc()->factoryReset();

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->error(), operation->errorString());
        }
    });
}

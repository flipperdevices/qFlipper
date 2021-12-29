#include "factoryresetutiloperation.h"

#include <QTimer>

#include "flipperzero/devicestate.h"
#include "flipperzero/commandinterface.h"
#include "flipperzero/rpc/systemfactoryresetoperation.h"

#define CALL_LATER(obj, func) (QTimer::singleShot(0, obj, func))

using namespace Flipper;
using namespace Zero;

FactoryResetUtilOperation::FactoryResetUtilOperation(CommandInterface *cli, DeviceState *deviceState, QObject *parent):
    AbstractUtilityOperation(cli, deviceState, parent)
{}

const QString FactoryResetUtilOperation::description() const
{
    return QStringLiteral("Factory reset (Utility) @%1").arg(deviceState()->name());
}

void FactoryResetUtilOperation::advanceOperationState()
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
        CALL_LATER(this, &FactoryResetUtilOperation::advanceOperationState);
    } else {
        startTimeout();
    }
}

void FactoryResetUtilOperation::resetDevice()
{
    deviceState()->setStatusString(QStringLiteral("Performing factory reset..."));

    connect(deviceState(), &DeviceState::isOnlineChanged, this, &FactoryResetUtilOperation::onDeviceOnlineChanged);

    auto *operation = cli()->factoryReset();

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->errorString());
        }
    });
}

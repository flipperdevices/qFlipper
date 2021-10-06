#include "restartoperation.h"

#include <QTimer>

#include "flipperzero/devicestate.h"
#include "flipperzero/commandinterface.h"
#include "flipperzero/cli/rebootoperation.h"

#define CALL_LATER(obj, func) (QTimer::singleShot(0, obj, func))

using namespace Flipper;
using namespace Zero;

RestartOperation::RestartOperation(CommandInterface *cli, DeviceState *deviceState, QObject *parent):
    AbstractUtilityOperation(cli, deviceState, parent)
{}

const QString RestartOperation::description() const
{
    const auto &model = deviceState()->deviceInfo().model;
    const auto &name = deviceState()->deviceInfo().name;

    return QStringLiteral("Restart device @%1 %2").arg(model, name);
}

void RestartOperation::advanceOperationState()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(RestartOperation::WaitingForOSBoot);
        rebootDevice();

    } else if(operationState() == RestartOperation::WaitingForOSBoot) {
        disconnect(deviceState(), &DeviceState::isOnlineChanged, this, &RestartOperation::onDeviceOnlineChanged);
        finish();

    } else {}
}

void RestartOperation::onDeviceOnlineChanged()
{
    if(deviceState()->isOnline()) {
        CALL_LATER(this, &RestartOperation::advanceOperationState);
    } else {
        startTimeout();
    }
}

void RestartOperation::rebootDevice()
{
    deviceState()->setStatusString(QStringLiteral("Restarting device..."));

    connect(deviceState(), &DeviceState::isOnlineChanged, this, &RestartOperation::onDeviceOnlineChanged);

    auto *operation = cli()->reboot();

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->errorString());
        }
    });
}

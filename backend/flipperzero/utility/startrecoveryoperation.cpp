#include "startrecoveryoperation.h"

#include <QTimer>

#include "flipperzero/devicestate.h"
#include "flipperzero/commandinterface.h"
#include "flipperzero/cli/dfuoperation.h"

#define CALL_LATER(obj, func) (QTimer::singleShot(0, obj, func))

using namespace Flipper;
using namespace Zero;

StartRecoveryOperation::StartRecoveryOperation(CommandInterface *cli, DeviceState *deviceState, QObject *parent):
    AbstractUtilityOperation(cli, deviceState, parent)
{}

const QString StartRecoveryOperation::description() const
{
    return QStringLiteral("Start Recovery Mode @%1").arg(deviceState()->name());
}

void StartRecoveryOperation::advanceOperationState()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(StartRecoveryOperation::WaitingForRecovery);
        startRecoveryMode();

    } else if(operationState() == StartRecoveryOperation::WaitingForRecovery) {
        disconnect(deviceState(), &DeviceState::isOnlineChanged, this, &StartRecoveryOperation::onDeviceOnlineChanged);
        finish();

    } else {}
}

void StartRecoveryOperation::onDeviceOnlineChanged()
{
    if(deviceState()->isOnline()) {
        CALL_LATER(this, &StartRecoveryOperation::advanceOperationState);
    } else {
        startTimeout();
    }
}

void StartRecoveryOperation::startRecoveryMode()
{
    deviceState()->setStatusString(QStringLiteral("Starting Recovery mode..."));

    connect(deviceState(), &DeviceState::isOnlineChanged, this, &StartRecoveryOperation::onDeviceOnlineChanged);

    auto *operation = cli()->startRecoveryMode();

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->errorString());
        }
    });
}

#include "abstractutilityoperation.h"

#include <QTimer>

#include "flipperzero/recovery.h"
#include "flipperzero/devicestate.h"

using namespace Flipper;
using namespace Zero;

AbstractUtilityOperation::AbstractUtilityOperation(CommandInterface *cli, DeviceState *deviceState, QObject *parent):
    AbstractOperation(parent),
    m_rpc(cli),
    m_deviceState(deviceState)
{}

void AbstractUtilityOperation::start()
{
    if(operationState() != AbstractOperation::Ready) {
        finishWithError(BackendError::UnknownError, QStringLiteral("Trying to start an operation that is either already running or has finished."));
    } else {
        advanceOperationState();
    }
}

CommandInterface *AbstractUtilityOperation::rpc() const
{
    return m_rpc;
}

DeviceState *AbstractUtilityOperation::deviceState() const
{
    return m_deviceState;
}

void AbstractUtilityOperation::advanceOperationState()
{
    QTimer::singleShot(0, this, &AbstractUtilityOperation::nextStateLogic);
}

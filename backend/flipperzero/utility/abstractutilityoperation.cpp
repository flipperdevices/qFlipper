#include "abstractutilityoperation.h"

#include <QTimer>

#include "flipperzero/recovery.h"
#include "flipperzero/devicestate.h"

#define CALL_LATER(obj, func) (QTimer::singleShot(0, obj, func))

using namespace Flipper;
using namespace Zero;

AbstractUtilityOperation::AbstractUtilityOperation(CommandInterface *cli, DeviceState *deviceState, QObject *parent):
    AbstractOperation(parent),
    m_cli(cli),
    m_deviceState(deviceState)
{}

void AbstractUtilityOperation::start()
{
    if(operationState() != AbstractOperation::Ready) {
        finishWithError(QStringLiteral("Trying to start an operation that is either already running or has finished."));
    } else {
        CALL_LATER(this, &AbstractUtilityOperation::advanceOperationState);
    }
}

CommandInterface *AbstractUtilityOperation::cli() const
{
    return m_cli;
}

DeviceState *AbstractUtilityOperation::deviceState() const
{
    return m_deviceState;
}

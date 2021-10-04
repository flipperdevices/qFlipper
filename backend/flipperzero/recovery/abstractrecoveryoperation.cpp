#include "abstractrecoveryoperation.h"

#include <QTimer>

#include "flipperzero/recovery.h"
#include "flipperzero/devicestate.h"

#define CALL_LATER(obj, func) (QTimer::singleShot(0, obj, func))

using namespace Flipper;
using namespace Zero;

AbstractRecoveryOperation::AbstractRecoveryOperation(Recovery *recovery, QObject *parent):
    AbstractOperation(parent),
    m_recovery(recovery)
{}

void AbstractRecoveryOperation::start()
{
    if(operationState() == BasicOperationState::Ready) {
        connect(m_recovery->deviceState(), &DeviceState::isOnlineChanged, this, &AbstractRecoveryOperation::onDeviceOnlineChanged);
        CALL_LATER(this, &AbstractRecoveryOperation::advanceOperationState);
    } else {
        finishWithError(QStringLiteral("Trying to start an operation that is either already running or has finished."));
    }
}

void AbstractRecoveryOperation::finish()
{
    disconnect(m_recovery->deviceState(), &DeviceState::isOnlineChanged, this, &AbstractRecoveryOperation::onDeviceOnlineChanged);
    emit finished();
}

void AbstractRecoveryOperation::onDeviceOnlineChanged()
{
    if(m_recovery->deviceState()->isOnline()) {
        stopTimeout();
        CALL_LATER(this, &AbstractRecoveryOperation::advanceOperationState);
    } else {
        startTimeout();
    }
}

Recovery *AbstractRecoveryOperation::recovery() const
{
    return m_recovery;
}

DeviceState *AbstractRecoveryOperation::deviceState() const
{
    return m_recovery->deviceState();
}

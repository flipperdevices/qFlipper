#include "abstractrecoveryoperation.h"

#include <QTimer>

#include "flipperzero/recovery.h"
#include "flipperzero/devicestate.h"

using namespace Flipper;
using namespace Zero;

AbstractRecoveryOperation::AbstractRecoveryOperation(Recovery *recovery, QObject *parent):
    AbstractOperation(parent),
    m_recovery(recovery)
{}

void AbstractRecoveryOperation::start()
{
    if(operationState() != AbstractOperation::Ready) {
        finishWithError(BackendError::RecoveryError, QStringLiteral("Trying to start an operation that is either already running or has finished."));
    } else {
        connect(m_recovery->deviceState(), &DeviceState::isOnlineChanged, this, &AbstractRecoveryOperation::onDeviceOnlineChanged);
        QTimer::singleShot(0, this, &AbstractRecoveryOperation::nextStateLogic);
    }
}

void AbstractRecoveryOperation::finish()
{
    disconnect(m_recovery->deviceState(), &DeviceState::isOnlineChanged, this, &AbstractRecoveryOperation::onDeviceOnlineChanged);
    AbstractOperation::finish();
}

void AbstractRecoveryOperation::onDeviceOnlineChanged()
{
    if(deviceState()->isOnline()) {
        stopTimeout();
        advanceOperationState();
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

void AbstractRecoveryOperation::advanceOperationState()
{
    QTimer::singleShot(0, this, &AbstractRecoveryOperation::nextStateLogic);
}

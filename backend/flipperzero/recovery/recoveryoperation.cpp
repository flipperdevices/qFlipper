#include "recoveryoperation.h"

#include <QTimer>

#include "flipperzero/recovery.h"
#include "flipperzero/devicestate.h"

#define CALL_LATER(obj, func) (QTimer::singleShot(0, obj, func))

using namespace Flipper;
using namespace Zero;

RecoveryOperation::RecoveryOperation(Recovery *recovery, QObject *parent):
    AbstractOperation(parent),
    m_recovery(recovery)
{}

void RecoveryOperation::start()
{
    if(operationState() == BasicOperationState::Ready) {
        connect(m_recovery->deviceState(), &DeviceState::isOnlineChanged, this, &RecoveryOperation::onDeviceOnlineChanged);
        CALL_LATER(this, &RecoveryOperation::doNextOperationState);
    } else {
        finishWithError(QStringLiteral("Trying to start an operation that is either already running or has finished."));
    }
}

void RecoveryOperation::finish()
{
    disconnect(m_recovery->deviceState(), &DeviceState::isOnlineChanged, this, &RecoveryOperation::onDeviceOnlineChanged);
    emit finished();
}

void RecoveryOperation::onDeviceOnlineChanged()
{
    if(m_recovery->deviceState()->isOnline()) {
        stopTimeout();
        CALL_LATER(this, &RecoveryOperation::doNextOperationState);
    } else {
        startTimeout();
    }
}

Recovery *RecoveryOperation::recovery() const
{
    return m_recovery;
}

DeviceState *RecoveryOperation::deviceState() const
{
    return m_recovery->deviceState();
}

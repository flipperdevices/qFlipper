#include "abstractrecoveryoperation.h"

#include <QTimer>
#include <QDebug>
#include <QLoggingCategory>

#include "flipperzero/recovery.h"
#include "flipperzero/devicestate.h"

Q_DECLARE_LOGGING_CATEGORY(LOG_RECOVERY)

using namespace Flipper;
using namespace Zero;

AbstractRecoveryOperation::AbstractRecoveryOperation(Recovery *recovery, QObject *parent):
    AbstractOperation(parent),
    m_recovery(recovery)
{}

void AbstractRecoveryOperation::start()
{
    if(operationState() != AbstractOperation::Ready) {
        finishWithError(QStringLiteral("Trying to start an operation that is either already running or has finished."));
    } else {
        connect(m_recovery->deviceState(), &DeviceState::isOnlineChanged, this, &AbstractRecoveryOperation::onDeviceOnlineChanged);
        QTimer::singleShot(0, this, &AbstractRecoveryOperation::advanceOperationState);
    }
}

void AbstractRecoveryOperation::finish()
{
    disconnect(m_recovery->deviceState(), &DeviceState::isOnlineChanged, this, &AbstractRecoveryOperation::onDeviceOnlineChanged);
    AbstractOperation::finish();
}

void AbstractRecoveryOperation::onDeviceOnlineChanged()
{
    qCDebug(LOG_RECOVERY) << (deviceState()->isOnline() ?
        "Device went back online" : "Device is offline");

    if(deviceState()->isOnline()) {
        stopTimeout();
        QTimer::singleShot(0, this, &AbstractRecoveryOperation::advanceOperationState);

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

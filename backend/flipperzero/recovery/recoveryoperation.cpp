#include "recoveryoperation.h"

#include <QTimer>

#define CALL_LATER(obj, func) (QTimer::singleShot(0, obj, func))

using namespace Flipper;
using namespace Zero;

RecoveryOperation::RecoveryOperation(Recovery *recovery, QObject *parent):
    AbstractOperation(parent),
    m_recovery(recovery)
{}

void RecoveryOperation::start()
{
    if(operationState() == BasicState::Ready) {
        CALL_LATER(this, &RecoveryOperation::doNextOperationState);
    } else {
        finishWithError(QStringLiteral("Trying to start an operation that is either already running or has finished."));
    }
}

void RecoveryOperation::finish()
{
    emit finished();
}

void RecoveryOperation::onDeviceOnlineChanged(bool isOnline)
{
    if(isOnline) {
        CALL_LATER(this, &RecoveryOperation::doNextOperationState);
    } else {
        startTimeout();
    }
}

Recovery *RecoveryOperation::recovery() const
{
    return m_recovery;
}

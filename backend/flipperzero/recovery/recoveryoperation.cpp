#include "recoveryoperation.h"

#include <QTimer>

#define CALL_LATER(obj, func) (QTimer::singleShot(0, obj, func))

using namespace Flipper;
using namespace Zero;

RecoveryOperation::RecoveryOperation(Recovery *recovery, QObject *parent):
    AbstractOperation(parent),
    m_recovery(recovery)
{}

RecoveryOperation::~RecoveryOperation()
{}

void RecoveryOperation::start()
{
    if(state() != BasicState::Ready) {
        finishWithError(QStringLiteral("Trying to start an operation that is either already running or has finished."));
        return;
    }

    CALL_LATER(this, &RecoveryOperation::transitionToNextState);
}

void RecoveryOperation::finish()
{
    emit finished();
}

void RecoveryOperation::onDeviceOnline()
{
    transitionToNextState();
}

void RecoveryOperation::onDeviceOffline()
{
    startTimeout();
}

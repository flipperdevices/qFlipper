#include "setbootmodeoperation.h"

#include <QDebug>
#include <QLoggingCategory>

#include "flipperzero/devicestate.h"
#include "flipperzero/recovery.h"

Q_DECLARE_LOGGING_CATEGORY(LOG_RECOVERY)

using namespace Flipper;
using namespace Zero;

SetBootModeOperation::SetBootModeOperation(Recovery *recovery, QObject *parent):
    AbstractRecoveryOperation(recovery, parent)
{}

const QString SetBootModeOperation::description() const
{
    return QStringLiteral("Set %1 boot mode @%2").arg(typeString(), deviceState()->name());
}

void SetBootModeOperation::advanceOperationState()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(SetBootModeOperation::WaitingForBoot);
        setBootMode();

    } else if(operationState() == SetBootModeOperation::WaitingForBoot) {
        finish();
    }
}

void SetBootModeOperation::onOperationTimeout()
{
    if(!deviceState()->isOnline()) {
        finishWithError(QStringLiteral("Failed to set %1 mode: operation timeout").arg(typeString()));
    } else {
        // TODO: Check that the boot mode was correctly set, if not, repeat
        qCDebug(LOG_RECOVERY) << "Timeout with an online device, assuming it is still functional";
        advanceOperationState();
    }
}

void SetBootModeOperation::setBootMode()
{
    if(!recovery()->setBootMode((Recovery::BootMode)bootMode())) {
        finishWithError(recovery()->errorString());
    } else {
        // Do not rely on the device disconnecting and thus triggering the timeout
        startTimeout();
    }
}

SetRecoveryBootOperation::SetRecoveryBootOperation(Recovery *recovery, QObject *parent):
    SetBootModeOperation(recovery, parent)
{}

int SetRecoveryBootOperation::bootMode() const
{
    return (int)Recovery::BootMode::DFUOnly;
}

const QString SetRecoveryBootOperation::typeString() const
{
    return QStringLiteral("Recovery");
}

SetOSBootOperation::SetOSBootOperation(Recovery *recovery, QObject *parent):
    SetBootModeOperation(recovery, parent)
{}

int SetOSBootOperation::bootMode() const
{
    return (int)Recovery::BootMode::Normal;
}

const QString SetOSBootOperation::typeString() const
{
    return QStringLiteral("OS");
}

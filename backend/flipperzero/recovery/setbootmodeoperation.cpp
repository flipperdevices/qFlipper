#include "setbootmodeoperation.h"

#include "flipperzero/devicestate.h"
#include "flipperzero/recovery.h"

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
    finishWithError(QStringLiteral("Failed to set %1 mode: operation timeout").arg(typeString()));
}

void SetBootModeOperation::setBootMode()
{
    if(!recovery()->setBootMode((Recovery::BootMode)bootMode())) {
        finishWithError(recovery()->errorString());
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

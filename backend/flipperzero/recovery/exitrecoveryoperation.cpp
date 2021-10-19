#include "exitrecoveryoperation.h"

#include "flipperzero/recovery.h"
#include "flipperzero/devicestate.h"

using namespace Flipper;
using namespace Zero;

ExitRecoveryOperation::ExitRecoveryOperation(Recovery *recovery, QObject *parent):
    AbstractRecoveryOperation(recovery, parent)
{}

const QString ExitRecoveryOperation::description() const
{
    return QStringLiteral("Exit Recovery Mode @%1").arg(deviceState()->name());
}

void ExitRecoveryOperation::advanceOperationState()
{
    if(operationState() == BasicOperationState::Ready) {
        setOperationState(OperationState::WaitingForOnline);

        if(!recovery()->exitRecoveryMode()) {
            finishWithError(recovery()->errorString());
        }

    } else if(operationState() == OperationState::WaitingForOnline) {
        finish();
    }
}

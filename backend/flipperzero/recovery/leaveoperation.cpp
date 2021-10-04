#include "leaveoperation.h"

#include "flipperzero/recovery.h"
#include "flipperzero/devicestate.h"

using namespace Flipper;
using namespace Zero;

LeaveOperation::LeaveOperation(Recovery *recovery, QObject *parent):
    AbstractRecoveryOperation(recovery, parent)
{}

const QString LeaveOperation::description() const
{
    return QStringLiteral("Exit Recovery Mode");
}

void LeaveOperation::advanceOperationState()
{
    if(operationState() == BasicOperationState::Ready) {
        setOperationState(OperationState::WaitingForOnline);

        deviceState()->setStatusString(tr("Exiting recovery mode..."));

        if(!recovery()->exitRecoveryMode()) {
            finishWithError(recovery()->errorString());
        }

    } else if(operationState() == OperationState::WaitingForOnline) {
        finish();
    }
}

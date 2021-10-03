#include "leaveoperation.h"

#include "flipperzero/recovery.h"
#include "flipperzero/devicestate.h"

using namespace Flipper;
using namespace Zero;

LeaveOperation::LeaveOperation(Recovery *recovery, QObject *parent):
    RecoveryOperation(recovery, parent)
{}

const QString LeaveOperation::description() const
{
    return QStringLiteral("Exit Recovery Mode");
}

void LeaveOperation::doNextOperationState()
{
    if(operationState() == BasicOperationState::Ready) {
        setOperationState(OperationState::WaitingForOnline);

        deviceState()->setStatusString(tr("Exiting recovery mode..."));

        if(!recovery()->leaveDFU()) {
            finishWithError(recovery()->errorString());
        }

    } else if(operationState() == OperationState::WaitingForOnline) {
        finish();
    }
}

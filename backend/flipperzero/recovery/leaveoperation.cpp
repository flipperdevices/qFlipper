#include "leaveoperation.h"

#include "flipperzero/recovery.h"

using namespace Flipper;
using namespace Zero;

LeaveOperation::LeaveOperation(Recovery *recovery, QObject *parent):
    RecoveryOperation(recovery, parent)
{}

const QString LeaveOperation::description() const
{
    return QStringLiteral("Leave Recovery Mode");
}

void LeaveOperation::doNextOperationState()
{
    if(operationState() == BasicState::Ready) {
        setOperationState(State::WaitingForOnline);

//        setMessage(tr("Exiting recovery mode..."));

        if(!recovery()->leaveDFU()) {
            finishWithError(recovery()->errorString());
        }

    } else if(operationState() == State::WaitingForOnline) {
        finish();
    }
}

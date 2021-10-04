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
    const auto &model = deviceState()->deviceInfo().model;
    const auto &name = deviceState()->deviceInfo().name;

    return QStringLiteral("Exit Recovery Mode @%1 %2").arg(model, name);
}

void ExitRecoveryOperation::advanceOperationState()
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

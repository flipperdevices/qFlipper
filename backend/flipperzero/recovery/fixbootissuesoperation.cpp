#include "fixbootissuesoperation.h"

#include "flipperzero/recovery.h"
#include "flipperzero/devicestate.h"

using namespace Flipper;
using namespace Zero;

FixBootIssuesOperation::FixBootIssuesOperation(Recovery *recovery, QObject *parent):
    RecoveryOperation(recovery, parent)
{
//    recovery->setPersistent(true);
//    recovery->setMessage(QStringLiteral("Fix boot issues operation pending..."));
}

FixBootIssuesOperation::~FixBootIssuesOperation()
{
//    deviceState()->setPersistent(false);
}

const QString FixBootIssuesOperation::description() const
{
    const auto &model = recovery()->deviceState()->deviceInfo().model;
    const auto &name = recovery()->deviceState()->deviceInfo().name;

    return QStringLiteral("Fix boot issues @%1 %2").arg(model, name);
}

void FixBootIssuesOperation::transitionToNextState()
{
    if(!deviceState()->isOnline()) {
        startTimeout();
        return;
    }

    stopTimeout();

    if(operationState() == AbstractOperation::Ready) {
        setOperationState(FixBootIssuesOperation::StartingWirelessStack);
        startWirelessStack();

    } else if(operationState() == FixBootIssuesOperation::StartingWirelessStack) {
        setOperationState(FixBootIssuesOperation::FixingBootMode);
        fixBootMode();

    } else if(operationState() == FixBootIssuesOperation::FixingBootMode) {
        setOperationState(AbstractOperation::Finished);
        finish();

    } else {
        finishWithError(QStringLiteral("Unexpected state."));
//        deviceState()->setError(errorString());
    }
}

void FixBootIssuesOperation::onOperationTimeout()
{
    QString msg;

    if(operationState() == FixBootIssuesOperation::StartingWirelessStack) {
        msg = QStringLiteral("Failed to start the Wireless Stack: Operation timeout.");
    } else if(operationState() == FixBootIssuesOperation::FixingBootMode) {
        msg = QStringLiteral("Failed to set the Option Bytes: Operation timeout.");
    } else {
        msg = QStringLiteral("Should not have timed out here, probably a bug.");
    }

    finishWithError(msg);
//    deviceState()->setError(errorString());
}

void FixBootIssuesOperation::startWirelessStack()
{
    const auto wirelessStatus = recovery()->wirelessStatus();

    if(wirelessStatus == Recovery::WirelessStatus::FUSRunning) {
        if(!recovery()->startWirelessStack()) {
            finishWithError(deviceState()->errorString());
        } else if(recovery()->wirelessStatus() == Recovery::WirelessStatus::UnhandledState) {
            transitionToNextState();
        } else {}

    } else if(wirelessStatus == Recovery::WirelessStatus::WSRunning) {
        transitionToNextState();
    } else if(wirelessStatus == Recovery::WirelessStatus::UnhandledState) {
        finishWithError(QStringLiteral("Unhandled state. Probably a BUG."));
//        deviceState()->setError(errorString());
    } else {
        finishWithError(QStringLiteral("Failed to get Wireless core status."));
//        deviceState()->setError(errorString());
    }
}

void FixBootIssuesOperation::fixBootMode()
{
    if(!deviceState()->isRecoveryMode()) {
        transitionToNextState();
    } else if (!recovery()->setBootMode(Recovery::BootMode::Normal)) {
        finishWithError(deviceState()->errorString());
    } else {}
}

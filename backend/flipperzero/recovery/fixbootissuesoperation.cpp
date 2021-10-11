#include "fixbootissuesoperation.h"

#include "flipperzero/recovery.h"
#include "flipperzero/devicestate.h"

using namespace Flipper;
using namespace Zero;

FixBootIssuesOperation::FixBootIssuesOperation(Recovery *recovery, QObject *parent):
    AbstractRecoveryOperation(recovery, parent)
{}

FixBootIssuesOperation::~FixBootIssuesOperation()
{}

const QString FixBootIssuesOperation::description() const
{
    return QStringLiteral("Fix boot issues @%1").arg(deviceState()->name());
}

void FixBootIssuesOperation::advanceOperationState()
{
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
}

void FixBootIssuesOperation::startWirelessStack()
{
    const auto wirelessStatus = recovery()->wirelessStatus();

    if(wirelessStatus == Recovery::WirelessStatus::FUSRunning) {
        if(!recovery()->startWirelessStack()) {
            finishWithError(deviceState()->errorString());
        } else if(recovery()->wirelessStatus() == Recovery::WirelessStatus::UnhandledState) {
            advanceOperationState();
        } else {}

    } else if(wirelessStatus == Recovery::WirelessStatus::WSRunning) {
        advanceOperationState();
    } else if(wirelessStatus == Recovery::WirelessStatus::UnhandledState) {
        finishWithError(QStringLiteral("Unhandled state. Probably a BUG."));
    } else {
        finishWithError(QStringLiteral("Failed to get Wireless core status."));
    }
}

void FixBootIssuesOperation::fixBootMode()
{
    if(!deviceState()->isRecoveryMode()) {
        advanceOperationState();
    } else if (!recovery()->setBootMode(Recovery::BootMode::Normal)) {
        finishWithError(recovery()->errorString());
    } else {}
}

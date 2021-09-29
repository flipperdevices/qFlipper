#include "fixbootissuesoperation.h"

#include "flipperzero/flipperzero.h"
#include "flipperzero/recoveryinterface.h"

using namespace Flipper;
using namespace Zero;

FixBootIssuesOperation::FixBootIssuesOperation(FlipperZero *device, QObject *parent):
    FlipperZeroOperation(device, parent)
{
    device->setPersistent(true);
    device->setMessage(QStringLiteral("Fix boot issues operation pending..."));
}

FixBootIssuesOperation::~FixBootIssuesOperation()
{
    device()->setPersistent(false);
}

const QString FixBootIssuesOperation::description() const
{
    return QStringLiteral("Fix boot issues @%1 %2").arg(device()->model(), device()->name());
}

void FixBootIssuesOperation::transitionToNextState()
{
    if(!device()->isOnline()) {
        startTimeout();
        return;
    }

    stopTimeout();

    if(state() == AbstractOperation::Ready) {
        setState(FixBootIssuesOperation::StartingWirelessStack);
        startWirelessStack();

    } else if(state() == FixBootIssuesOperation::StartingWirelessStack) {
        setState(FixBootIssuesOperation::FixingBootMode);
        fixBootMode();

    } else if(state() == FixBootIssuesOperation::FixingBootMode) {
        setState(AbstractOperation::Finished);
        finish();

    } else {
        finishWithError(QStringLiteral("Unexpected state."));
        device()->setError(errorString());
    }
}

void FixBootIssuesOperation::onOperationTimeout()
{
    QString msg;

    if(state() == FixBootIssuesOperation::StartingWirelessStack) {
        msg = QStringLiteral("Failed to start the Wireless Stack: Operation timeout.");
    } else if(state() == FixBootIssuesOperation::FixingBootMode) {
        msg = QStringLiteral("Failed to set the Option Bytes: Operation timeout.");
    } else {
        msg = QStringLiteral("Should not have timed out here, probably a bug.");
    }

    finishWithError(msg);
    device()->setError(errorString());
}

void FixBootIssuesOperation::startWirelessStack()
{
    const auto wirelessStatus = device()->recovery()->wirelessStatus();

    if(wirelessStatus == RecoveryInterface::WirelessStatus::FUSRunning) {
        if(!device()->recovery()->startWirelessStack()) {
            finishWithError(device()->errorString());
        } else if(device()->recovery()->wirelessStatus() == RecoveryInterface::WirelessStatus::UnhandledState) {
            transitionToNextState();
        } else {}

    } else if(wirelessStatus == RecoveryInterface::WirelessStatus::WSRunning) {
        transitionToNextState();
    } else if(wirelessStatus == RecoveryInterface::WirelessStatus::UnhandledState) {
        finishWithError(QStringLiteral("Unhandled state. Probably a BUG."));
        device()->setError(errorString());
    } else {
        finishWithError(QStringLiteral("Failed to get Wireless core status."));
        device()->setError(errorString());
    }
}

void FixBootIssuesOperation::fixBootMode()
{
    if(!device()->isDFU()) {
        transitionToNextState();
    } else if (!device()->recovery()->setBootMode(RecoveryInterface::BootMode::Normal)) {
        finishWithError(device()->errorString());
    } else {}
}

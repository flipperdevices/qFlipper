#include "fixbootissuesoperation.h"

#include "flipperzero/flipperzero.h"
#include "flipperzero/recoverycontroller.h"

using namespace Flipper;
using namespace Zero;

FixBootIssuesOperation::FixBootIssuesOperation(FlipperZero *device, QObject *parent):
    Operation(device, parent)
{
    device->setPersistent(true);
    device->setStatusMessage(QObject::tr("Fix boot issues operation pending..."));
}

FixBootIssuesOperation::~FixBootIssuesOperation()
{
    device()->setPersistent(false);
}

const QString FixBootIssuesOperation::name() const
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
        setError(QStringLiteral("Unexpected state."));
    }
}

void FixBootIssuesOperation::onOperationTimeout()
{
    switch(state()) {
    case FixBootIssuesOperation::StartingWirelessStack:
        setError(QStringLiteral("Failed to start the Wireless Stack: Operation timeout."));
        break;
    case FixBootIssuesOperation::FixingBootMode:
        setError(QStringLiteral("Failed to set the Option Bytes: Operation timeout."));
        break;
    default:
        setError(QStringLiteral("Should not have timed out here, probably a bug."));
    }
}

void FixBootIssuesOperation::startWirelessStack()
{
    const auto wirelessStatus = device()->recovery()->wirelessStatus();

    if(wirelessStatus == RecoveryController::WirelessStatus::FUSRunning) {
        if(!device()->recovery()->startWirelessStack()) {
            setError(QStringLiteral("Failed to start the Wireless Stack."));
        } else if(device()->recovery()->wirelessStatus() == RecoveryController::WirelessStatus::UnhandledState) {
            transitionToNextState();
        } else {}

    } else if(wirelessStatus == RecoveryController::WirelessStatus::WSRunning) {
        transitionToNextState();
    } else if(wirelessStatus == RecoveryController::WirelessStatus::UnhandledState) {
        setError(QStringLiteral("Unhandled state. Probably a BUG."));
    } else {
        setError(QStringLiteral("Failed to get Wireless core status."));
    }
}

void FixBootIssuesOperation::fixBootMode()
{
    if(!device()->isDFU()) {
        transitionToNextState();
    } else if (!device()->recovery()->setBootMode(RecoveryController::BootMode::Normal)) {
        setError(QStringLiteral("Failed to set the Option Bytes."));
    } else {}
}

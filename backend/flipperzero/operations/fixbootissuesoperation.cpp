#include "fixbootissuesoperation.h"

#include "flipperzero/flipperzero.h"

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
    const auto wirelessStatus = device()->wirelessStatus();

    if(wirelessStatus == FlipperZero::WirelessStatus::FUSRunning) {
        if(!device()->startWirelessStack()) {
            setError(QStringLiteral("Failed to start the Wireless Stack."));
        } else if(device()->wirelessStatus() == FlipperZero::WirelessStatus::UnhandledState) {
            transitionToNextState();
        } else {}

    } else if(wirelessStatus == FlipperZero::WirelessStatus::WSRunning) {
        transitionToNextState();
    } else if(wirelessStatus == FlipperZero::WirelessStatus::UnhandledState) {
        setError(QStringLiteral("Unhandled state. Probably a BUG."));
    } else {
        setError(QStringLiteral("Failed to get Wireless core status."));
    }
}

void FixBootIssuesOperation::fixBootMode()
{
    if(!device()->isDFU()) {
        transitionToNextState();
    } else if (!device()->setBootMode(FlipperZero::BootMode::Normal)) {
        setError(QStringLiteral("Failed to set the Option Bytes."));
    } else {}
}

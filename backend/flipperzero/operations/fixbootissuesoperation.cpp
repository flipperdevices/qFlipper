#include "fixbootissuesoperation.h"

#include "flipperzero/flipperzero.h"

using namespace Flipper;
using namespace Zero;

FixBootIssuesOperation::FixBootIssuesOperation(FlipperZero *device, QObject *parent):
    AbstractFirmwareOperation(parent),
    m_device(device)
{
    m_device->setPersistent(true);
    m_device->setStatusMessage(QObject::tr("Fix boot issues operation pending..."));
}

FixBootIssuesOperation::~FixBootIssuesOperation()
{
    m_device->setPersistent(false);
}

const QString FixBootIssuesOperation::name() const
{
    return QStringLiteral("Fix boot issues @%1 %2").arg(m_device->model(), m_device->name());
}

void FixBootIssuesOperation::start()
{
    if(state() != Ready) {
        setError(QStringLiteral("Trying to start an operation that is either already running or has finished."));
        return;
    }

    connect(m_device, &FlipperZero::isOnlineChanged, this, &FixBootIssuesOperation::transitionToNextState);
    transitionToNextState();
}

void FixBootIssuesOperation::transitionToNextState()
{
    if(!m_device->isOnline()) {
        startTimeout();
        return;
    }

    stopTimeout();

    if(state() == AbstractFirmwareOperation::Ready) {
        setState(FixBootIssuesOperation::StartingWirelessStack);
        startWirelessStack();

    } else if(state() == FixBootIssuesOperation::StartingWirelessStack) {
        setState(FixBootIssuesOperation::FixingBootMode);
        fixBootMode();

    } else if(state() == FixBootIssuesOperation::FixingBootMode) {
        setState(AbstractFirmwareOperation::Finished);
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
    const auto wirelessStatus = m_device->wirelessStatus();

    if(wirelessStatus == FlipperZero::WirelessStatus::FUSRunning) {
        if(!m_device->startWirelessStack()) {
            setError(QStringLiteral("Failed to start the Wireless Stack."));
        } else if(m_device->wirelessStatus() == FlipperZero::WirelessStatus::UnhandledState) {
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
    if(!m_device->isDFU()) {
        transitionToNextState();
    } else if (!m_device->setBootMode(FlipperZero::BootMode::Normal)) {
        setError(QStringLiteral("Failed to set the Option Bytes."));
    } else{}
}

void FixBootIssuesOperation::finish()
{
    disconnect(m_device, &FlipperZero::isOnlineChanged, this, &FixBootIssuesOperation::transitionToNextState);
    emit finished();
}

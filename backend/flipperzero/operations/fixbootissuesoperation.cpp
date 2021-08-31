#include "fixbootissuesoperation.h"

#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

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
    if(state() != Idle) {
        setError(QStringLiteral("Trying to start an operation that is either already running or has finished."));
        return;
    }

    transitionToNextState();
}

void FixBootIssuesOperation::transitionToNextState()
{
    if(!m_device->isOnline()) {
        return;
    }

    stopTimeout();

    if(state() == AbstractFirmwareOperation::Idle) {
        setState(FixBootIssuesOperation::WaitForWirelessStack);
        connect(m_device, &FlipperZero::isOnlineChanged, this, &FixBootIssuesOperation::transitionToNextState);

        doStartWirelessStack();

    } else if(state() == FixBootIssuesOperation::WaitForWirelessStack) {
        setState(FixBootIssuesOperation::WaitForDeviceBoot);

        doFixBootMode();

    } else if(state() == FixBootIssuesOperation::WaitForDeviceBoot) {
        setState(AbstractFirmwareOperation::Finished);
        disconnect(m_device, &FlipperZero::isOnlineChanged, this, &FixBootIssuesOperation::transitionToNextState);
        emit finished();

    } else {
        setError(QStringLiteral("Unexpected state."));
    }
}

void FixBootIssuesOperation::onOperationTimeout()
{
    switch(state()) {
    case FixBootIssuesOperation::WaitForWirelessStack:
        setError(QStringLiteral("Failed to start the Wireless Stack: Operation timeout."));
        break;
    case FixBootIssuesOperation::WaitForDeviceBoot:
        setError(QStringLiteral("Failed to set the Option Bytes: Operation timeout."));
        break;
    default:
        setError(QStringLiteral("Should not have timed out here, probably a bug."));
    }
}

void FixBootIssuesOperation::doStartWirelessStack()
{
    const auto wirelessStatus = m_device->wirelessStatus();

    if(wirelessStatus == FlipperZero::WirelessStatus::FUSRunning) {
        if(!m_device->startWirelessStack()) {
            setError(QStringLiteral("Failed to start the Wireless Stack."));
        } else {
            startTimeout();
        }

    } else if(wirelessStatus == FlipperZero::WirelessStatus::WSRunning) {
        transitionToNextState();
    } else if(wirelessStatus == FlipperZero::WirelessStatus::UnhandledState) {
        setError(QStringLiteral("Unhandled state. Probably a BUG."));
    } else {
        setError(QStringLiteral("Failed to get Wireless core status."));
    }
}

void FixBootIssuesOperation::doFixBootMode()
{
    if(m_device->isDFU()) {
        if(!m_device->setBootMode(FlipperZero::BootMode::Normal)) {
            setError(QStringLiteral("Failed to set the Option Bytes."));
        } else {
            startTimeout();
        }

    } else {
        transitionToNextState();
    }
}

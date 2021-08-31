#include "wirelessstackupdateoperation.h"

#include <QIODevice>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

#include "flipperzero/flipperzero.h"

using namespace Flipper;
using namespace Zero;

WirelessStackUpdateOperation::WirelessStackUpdateOperation(FlipperZero *device, QIODevice *file, uint32_t targetAddress, QObject *parent):
    AbstractFirmwareOperation(parent),
    m_device(device),
    m_file(file),
    m_targetAddress(targetAddress)
{
    m_device->setPersistent(true);
    m_device->setStatusMessage(QObject::tr("Co-Processor firmware update pending..."));
}

WirelessStackUpdateOperation::~WirelessStackUpdateOperation()
{
    m_device->setPersistent(false);
    m_file->deleteLater();
}

const QString WirelessStackUpdateOperation::name() const
{
    return QString("Co-Processor Firmware Download @%1 %2").arg(m_device->model(), m_device->name());
}

void WirelessStackUpdateOperation::start()
{
    if(state() != Idle) {
        setError(QStringLiteral("Trying to start an operation that is either already running or has finished."));
        return;
    }

    transitionToNextState();
}

void WirelessStackUpdateOperation::transitionToNextState()
{
    if(!m_device->isOnline()) {
        return;
    }

    stopTimeout();

    if(state() == AbstractFirmwareOperation::Idle) {
        setState(State::WaitingForDFU);
        connect(m_device, &FlipperZero::isOnlineChanged, this, &WirelessStackUpdateOperation::transitionToNextState);
        doEnterDFUMode();

    } else if(state() == State::WaitingForDFU) {

    } else if(state() == State::WaitingForFirmwareBoot) {
        setState(AbstractFirmwareOperation::Finished);
        disconnect(m_device, &FlipperZero::isOnlineChanged, this, &WirelessStackUpdateOperation::transitionToNextState);
        emit finished();

    } else {
        setError(QStringLiteral("Unexpected state."));
    }
}

void WirelessStackUpdateOperation::onOperationTimeout()
{
    switch(state()) {
    case WirelessStackUpdateOperation::WaitingForDFU:
        setError(QStringLiteral("Failed to reach DFU mode: Operation timeout."));
        break;
    case WirelessStackUpdateOperation::WaitingForFirmwareBoot:
        setError(QStringLiteral("Failed to boot the device: Operation timeout."));
        break;
    default:
        setError(QStringLiteral("Should not have timed out here, probably a bug."));
    }
}

void WirelessStackUpdateOperation::doEnterDFUMode()
{
    if(m_device->isDFU()) {
        transitionToNextState();
    } else if(!m_device->enterDFU()) {
        setError(QStringLiteral("Failed to enter DFU mode."));
    } else {
        startTimeout();
    }
}

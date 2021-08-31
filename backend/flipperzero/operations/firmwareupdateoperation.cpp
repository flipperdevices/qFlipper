#include "firmwareupdateoperation.h"

#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

#include "flipperzero/flipperzero.h"

using namespace Flipper;
using namespace Zero;

FirmwareUpdateOperation::FirmwareUpdateOperation(FlipperZero *device, QIODevice *file, QObject *parent):
    AbstractFirmwareOperation(parent),
    m_device(device),
    m_file(file)
{
    m_device->setPersistent(true);
    m_device->setStatusMessage(QObject::tr("Firmware download pending..."));
}

FirmwareUpdateOperation::~FirmwareUpdateOperation()
{
    m_device->setPersistent(false);
    m_file->deleteLater();
}

const QString FirmwareUpdateOperation::name() const
{
    return QStringLiteral("Firmware Download @%1 %2").arg(m_device->model(), m_device->name());
}

void FirmwareUpdateOperation::start()
{
    if(state() != Idle) {
        setError(QStringLiteral("Trying to start an operation that is either already running or has finished."));
        return;
    }

    transitionToNextState();
}

void FirmwareUpdateOperation::transitionToNextState()
{
    if(!m_device->isOnline()) {
        return;
    }

    stopTimeout();

    if(state() == AbstractFirmwareOperation::Idle) {
        setState(State::WaitingForDFU);
        connect(m_device, &FlipperZero::isOnlineChanged, this, &FirmwareUpdateOperation::transitionToNextState);
        doEnterDFUMode();

    } else if(state() == State::WaitingForDFU) {
        setState(State::DownloadingFirmware);
        doDownloadFirmware();

    } else if(state() == State::DownloadingFirmware) {
        setState(State::WaitingForFirmwareBoot);
        doBootFirmware();

    } else if(state() == State::WaitingForFirmwareBoot) {
        setState(AbstractFirmwareOperation::Finished);
        disconnect(m_device, &FlipperZero::isOnlineChanged, this, &FirmwareUpdateOperation::transitionToNextState);
        emit finished();

    } else {
        setError(QStringLiteral("Unexpected state."));
    }
}

void FirmwareUpdateOperation::onOperationTimeout()
{
    switch(state()) {
    case FirmwareUpdateOperation::WaitingForDFU:
        setError(QStringLiteral("Failed to reach DFU mode: Operation timeout."));
        break;
    case FirmwareUpdateOperation::WaitingForFirmwareBoot:
        setError(QStringLiteral("Failed to reboot the device: Operation timeout."));
        break;
    default:
        setError(QStringLiteral("Should not have timed out here, probably a bug."));
    }
}

void FirmwareUpdateOperation::doEnterDFUMode()
{
    if(m_device->isDFU()) {
        transitionToNextState();
    } else if(!m_device->enterDFU()) {
        setError(QStringLiteral("Failed to detach the device."));
    } else {
        startTimeout();
    }
}

void FirmwareUpdateOperation::doDownloadFirmware()
{
    auto *watcher = new QFutureWatcher<bool>(this);

    connect(watcher, &QFutureWatcherBase::finished, this, [=]() {
        if(watcher->result()) {
            transitionToNextState();
        } else {
            setError(QStringLiteral("Failed to download the firmware."));
        }

        watcher->deleteLater();
    });

    watcher->setFuture(QtConcurrent::run(m_device, &FlipperZero::downloadFirmware, m_file));
}

void FirmwareUpdateOperation::doBootFirmware()
{
    if(!m_device->leaveDFU()) {
        setError(QStringLiteral("Failed to leave DFU mode."));
    } else {
        startTimeout();
    }
}

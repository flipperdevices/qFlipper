#include "firmwaredownloadoperation.h"

#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

#include "flipperzero/flipperzero.h"

using namespace Flipper;
using namespace Zero;

FirmwareDownloadOperation::FirmwareDownloadOperation(FlipperZero *device, QIODevice *file, QObject *parent):
    AbstractFirmwareOperation(parent),
    m_device(device),
    m_file(file)
{
    m_device->setPersistent(true);
    m_device->setStatusMessage(QObject::tr("Firmware download pending..."));
}

FirmwareDownloadOperation::~FirmwareDownloadOperation()
{
    m_device->setPersistent(false);
    m_file->deleteLater();
}

const QString FirmwareDownloadOperation::name() const
{
    return QStringLiteral("Firmware Download @%1 %2").arg(m_device->model(), m_device->name());
}

void FirmwareDownloadOperation::start()
{
    if(state() != Ready) {
        setError(QStringLiteral("Trying to start an operation that is either already running or has finished."));
        return;
    }

    connect(m_device, &FlipperZero::isOnlineChanged, this, &FirmwareDownloadOperation::transitionToNextState);
    transitionToNextState();
}

void FirmwareDownloadOperation::transitionToNextState()
{
    if(!m_device->isOnline()) {
        startTimeout();
        return;
    }

    stopTimeout();

    if(state() == AbstractFirmwareOperation::Ready) {
        setState(State::BootingToDFU);
        booToDFU();

    } else if(state() == State::BootingToDFU) {
        setState(State::DownloadingFirmware);
        downloadFirmware();

    } else if(state() == State::DownloadingFirmware) {
        setState(State::BootingToFirmware);
        bootToFirmware();

    } else if(state() == State::BootingToFirmware) {
        setState(AbstractFirmwareOperation::Finished);
        finish();

    } else {
        setError(QStringLiteral("Unexpected state."));
    }
}

void FirmwareDownloadOperation::onOperationTimeout()
{
    switch(state()) {
    case FirmwareDownloadOperation::BootingToDFU:
        setError(QStringLiteral("Failed to reach DFU mode: Operation timeout."));
        break;
    case FirmwareDownloadOperation::BootingToFirmware:
        setError(QStringLiteral("Failed to reboot the device: Operation timeout."));
        break;
    default:
        setError(QStringLiteral("Should not have timed out here, probably a bug."));
    }
}

void FirmwareDownloadOperation::booToDFU()
{
    if(m_device->isDFU()) {
        transitionToNextState();
    } else if(!m_device->bootToDFU()) {
        setError(QStringLiteral("Failed to detach the device."));
    }
}

void FirmwareDownloadOperation::downloadFirmware()
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

void FirmwareDownloadOperation::bootToFirmware()
{
    if(!m_device->reboot()) {
        setError(QStringLiteral("Failed to leave DFU mode."));
    }
}

void FirmwareDownloadOperation::finish()
{
    disconnect(m_device, &FlipperZero::isOnlineChanged, this, &FirmwareDownloadOperation::transitionToNextState);
    emit finished();
}

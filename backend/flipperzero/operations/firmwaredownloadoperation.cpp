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
    if(state() != Idle) {
        setError(QStringLiteral("Trying to start an operation that is either already running or has finished."));
        return;
    }

    transitionToNextState();
}

void FirmwareDownloadOperation::transitionToNextState()
{
    if(!m_device->isOnline()) {
        return;
    }

    stopTimeout();

    if(state() == AbstractFirmwareOperation::Idle) {
        setState(State::WaitingForDFU);

        connect(m_device, &FlipperZero::isOnlineChanged, this, &FirmwareDownloadOperation::transitionToNextState);

        if(m_device->isDFU()) {
            transitionToNextState();
        } else if(!m_device->enterDFU()) {
            setError(QStringLiteral("Failed to detach the device."));
            return;
        } else {
            startTimeout();
        }

    } else if(state() == State::WaitingForDFU) {
        setState(State::DownloadingFirmware);

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

    } else if(state() == State::DownloadingFirmware) {
        setState(State::WaitingForFirmwareBoot);

        if(!m_device->leaveDFU()) {
            setError(QStringLiteral("Failed to leave DFU mode."));
        } else {
            startTimeout();
        }

    } else if(state() == State::WaitingForFirmwareBoot) {
        setState(AbstractFirmwareOperation::Finished);

        disconnect(m_device, &FlipperZero::isOnlineChanged, this, &FirmwareDownloadOperation::transitionToNextState);
        emit finished();

    } else {
        setError(QStringLiteral("Unexpected state."));
    }
}

void FirmwareDownloadOperation::onOperationTimeout()
{
    switch(state()) {
    case FirmwareDownloadOperation::WaitingForDFU:
        setError(QStringLiteral("Failed to reach DFU mode: Operation timeout."));
        break;
    case FirmwareDownloadOperation::WaitingForFirmwareBoot:
        setError(QStringLiteral("Failed to reboot the device: Operation timeout."));
        break;
    default:
        setError(QStringLiteral("Should not have timed out here, probably a bug."));
    }
}

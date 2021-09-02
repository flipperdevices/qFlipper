#include "firmwaredownloadoperation.h"

#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

#include "flipperzero/flipperzero.h"
#include "flipperzero/recoverycontroller.h"

using namespace Flipper;
using namespace Zero;

FirmwareDownloadOperation::FirmwareDownloadOperation(FlipperZero *device, QIODevice *file, QObject *parent):
    Operation(device, parent),
    m_file(file)
{
    device->setMessage(QStringLiteral("Firmware download pending..."));
}

FirmwareDownloadOperation::~FirmwareDownloadOperation()
{
    m_file->deleteLater();
}

const QString FirmwareDownloadOperation::name() const
{
    return QStringLiteral("Firmware Download @%1 %2").arg(device()->model(), device()->name());
}

void FirmwareDownloadOperation::transitionToNextState()
{
    if(!device()->isOnline()) {
        startTimeout();
        return;
    }

    stopTimeout();

    if(state() == AbstractOperation::Ready) {
        setState(State::BootingToDFU);
        booToDFU();

    } else if(state() == State::BootingToDFU) {
        setState(State::DownloadingFirmware);
        downloadFirmware();

    } else if(state() == State::DownloadingFirmware) {
        setState(State::BootingToFirmware);
        bootToFirmware();

    } else if(state() == State::BootingToFirmware) {
        setState(AbstractOperation::Finished);
        finish();

    } else {
        setError(QStringLiteral("Unexpected state."));
        device()->setError(errorString());
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

    device()->setError(errorString());
}

void FirmwareDownloadOperation::booToDFU()
{
    if(device()->isDFU()) {
        transitionToNextState();
    } else if(!device()->bootToDFU()) {
        setError(device()->errorString());
    }
}

void FirmwareDownloadOperation::downloadFirmware()
{
    auto *watcher = new QFutureWatcher<bool>(this);

    connect(watcher, &QFutureWatcherBase::finished, this, [=]() {
        if(watcher->result()) {
            transitionToNextState();
        } else {
            setError(device()->recovery()->errorString());
        }

        watcher->deleteLater();
    });

    watcher->setFuture(QtConcurrent::run(device()->recovery(), &RecoveryController::downloadFirmware, m_file));
}

void FirmwareDownloadOperation::bootToFirmware()
{
    if(!device()->recovery()->leaveDFU()) {
        setError(device()->recovery()->errorString());
    }
}

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

const QString FirmwareDownloadOperation::description() const
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
        finishWithError(QStringLiteral("Unexpected state."));
        device()->setError(errorString());
    }
}

void FirmwareDownloadOperation::onOperationTimeout()
{
    QString msg;

    switch(state()) {
    case FirmwareDownloadOperation::BootingToDFU:
        msg = QStringLiteral("Failed to reach DFU mode: Operation timeout.");
        break;
    case FirmwareDownloadOperation::BootingToFirmware:
        msg = QStringLiteral("Failed to reboot the device: Operation timeout.");
        break;
    default:
        msg = QStringLiteral("Should not have timed out here, probably a bug.");
    }

    finishWithError(msg);
    device()->setError(msg);
}

void FirmwareDownloadOperation::booToDFU()
{
    if(device()->isDFU()) {
        transitionToNextState();
    } else if(!device()->bootToDFU()) {
        finishWithError(device()->errorString());
    }
}

void FirmwareDownloadOperation::downloadFirmware()
{
    auto *watcher = new QFutureWatcher<bool>(this);

    connect(watcher, &QFutureWatcherBase::finished, this, [=]() {
        if(watcher->result()) {
            transitionToNextState();
        } else {
            finishWithError(device()->errorString());
        }

        watcher->deleteLater();
    });

    watcher->setFuture(QtConcurrent::run(device()->recovery(), &RecoveryController::downloadFirmware, m_file));
}

void FirmwareDownloadOperation::bootToFirmware()
{
    if(!device()->recovery()->leaveDFU()) {
        finishWithError(device()->errorString());
    }
}

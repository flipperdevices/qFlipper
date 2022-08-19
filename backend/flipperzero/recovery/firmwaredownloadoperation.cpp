#include "firmwaredownloadoperation.h"

#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

#include "flipperzero/devicestate.h"
#include "flipperzero/recovery.h"

using namespace Flipper;
using namespace Zero;

FirmwareDownloadOperation::FirmwareDownloadOperation(Recovery *recovery, QIODevice *file, QObject *parent):
    AbstractRecoveryOperation(recovery, parent),
    m_file(file)
{}

const QString FirmwareDownloadOperation::description() const
{
    return QStringLiteral("Firmware Download @%1").arg(deviceState()->name());
}

void FirmwareDownloadOperation::nextStateLogic()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(State::DownloadingFirmware);
        downloadFirmware();
    } else if(operationState() == State::DownloadingFirmware) {
        finish();
    } else {
        finishWithError(BackendError::RecoveryError, QStringLiteral("Unexpected state."));
    }
}

void FirmwareDownloadOperation::downloadFirmware()
{
    auto *watcher = new QFutureWatcher<bool>(this);

    connect(watcher, &QFutureWatcherBase::finished, this, [=]() {
        if(watcher->result()) {
            advanceOperationState();
        } else {
            finishWithError(BackendError::RecoveryError, recovery()->errorString());
        }

        watcher->deleteLater();
    });

#if QT_VERSION < 0x060000
    watcher->setFuture(QtConcurrent::run(recovery(), &Recovery::downloadFirmware, m_file));
#else
    watcher->setFuture(QtConcurrent::run(&Recovery::downloadFirmware, recovery(), m_file));
#endif
}

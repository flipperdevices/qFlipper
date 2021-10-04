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

FirmwareDownloadOperation::~FirmwareDownloadOperation()
{
    // TODO: not hide the deletion of files?
    m_file->deleteLater();
}

const QString FirmwareDownloadOperation::description() const
{
    const auto &model = deviceState()->deviceInfo().model;
    const auto &name = deviceState()->deviceInfo().name;

    return QStringLiteral("Firmware Download @%1 %2").arg(model, name);
}

void FirmwareDownloadOperation::advanceOperationState()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(State::DownloadingFirmware);
        downloadFirmware();
    } else if(operationState() == State::DownloadingFirmware) {
        finish();
    } else {
        finishWithError(QStringLiteral("Unexpected state."));
    }
}

void FirmwareDownloadOperation::downloadFirmware()
{
    auto *watcher = new QFutureWatcher<bool>(this);

    connect(watcher, &QFutureWatcherBase::finished, this, [=]() {
        if(watcher->result()) {
            advanceOperationState();
        } else {
            finishWithError(recovery()->errorString());
        }

        watcher->deleteLater();
    });

    watcher->setFuture(QtConcurrent::run(recovery(), &Recovery::downloadFirmware, m_file));
}

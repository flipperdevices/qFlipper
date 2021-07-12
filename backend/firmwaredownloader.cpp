#include "firmwaredownloader.h"

#include <QUrl>
#include <QFile>
#include <QBuffer>
#include <QFutureWatcher>
#include <QSerialPortInfo>
#include <QtConcurrent/QtConcurrentRun>

#include "flipperzero/flipperzero.h"
#include "flipperzero/firmwareoperations.h"

#include "remotefilefetcher.h"
#include "macros.h"

using namespace Flipper;

FirmwareDownloader::FirmwareDownloader(QObject *parent):
    QObject(parent),
    m_state(State::Ready)
{}

void FirmwareDownloader::downloadLocalFile(FlipperZero *device, const QString &filePath)
{
    const auto localUrl = QUrl(filePath).toLocalFile();
    auto *file = new QFile(localUrl, this);

    enqueueOperation(new Flipper::Zero::FirmwareDownloadOperation(device, file));
}

void FirmwareDownloader::downloadRemoteFile(FlipperZero *device, const Updates::FileInfo &fileInfo)
{
    // TODO: Local cache on hard disk?
    auto *fetcher = new RemoteFileFetcher(this);

    connect(fetcher, &RemoteFileFetcher::finished, this, [=](const QByteArray &data) {
        auto *buf = new QBuffer(this);

        buf->open(QIODevice::ReadWrite);
        buf->write(data);
        buf->seek(0);
        buf->close();

        enqueueOperation(new Flipper::Zero::FirmwareDownloadOperation(device, buf));

        fetcher->deleteLater();
    });

    device->setStatusMessage(tr("Fetching"));
    fetcher->fetch(fileInfo);
}

void FirmwareDownloader::downloadLocalFUS(FlipperZero *device, const QString &filePath)
{
    Q_UNUSED(device)
    Q_UNUSED(filePath)
}

void FirmwareDownloader::downloadLocalWirelessStack(FlipperZero *device, const QString &filePath)
{
    const auto localUrl = QUrl(filePath).toLocalFile();
    auto *file = new QFile(localUrl, this);

    enqueueOperation(new Flipper::Zero::WirelessStackDownloadOperation(device, file));
}

void FirmwareDownloader::processQueue()
{
    if(m_operationQueue.isEmpty()) {
        m_state = State::Ready;
        return;
    }

    m_state = State::Running;

    auto *currentOperation = m_operationQueue.dequeue();
    auto *watcher = new QFutureWatcher<bool>(this);

    connect(watcher, &QFutureWatcherBase::finished, this, [=]() {
        info_msg(QString("Operation '%1' finished with status: %2").arg(currentOperation->name(), watcher->result() ? "SUCCESS" : "FAILURE"));

        delete currentOperation;
        processQueue();

        watcher->deleteLater();
    });

    watcher->setFuture(QtConcurrent::run(currentOperation, &AbstractFirmwareOperation::execute));
}

void FirmwareDownloader::enqueueOperation(AbstractFirmwareOperation *op)
{
    m_operationQueue.enqueue(op);

    if(m_state == State::Ready) {
        processQueue();
    }
}

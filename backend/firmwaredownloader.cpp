#include "firmwaredownloader.h"

#include <QUrl>
#include <QFile>
#include <QBuffer>
#include <QFutureWatcher>
#include <QSerialPortInfo>
#include <QtConcurrent/QtConcurrentRun>

#include <QDebug>

#include "flipperzero/flipperzero.h"
#include "remotefilefetcher.h"

using namespace Flipper;

FirmwareDownloader::FirmwareDownloader(QObject *parent):
    QObject(parent),
    m_state(State::Ready)
{}

void FirmwareDownloader::downloadLocalFile(FlipperZero *device, const QString &filePath)
{
    const auto localUrl = QUrl(filePath).toLocalFile();
    auto *file = new QFile(localUrl, this);

    enqueueRequest({device, file});
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

        enqueueRequest({device, buf});

        fetcher->deleteLater();
    });

    device->setStatusMessage(tr("Fetching"));
    fetcher->fetch(fileInfo);
}

void FirmwareDownloader::onDeviceConnected(FlipperZero *device)
{
    if(m_state != State::WaitingForDFU) {
        return;
    }

    m_currentRequest.device = device;
    processCurrentRequest();
}

void FirmwareDownloader::processQueue()
{
    if(m_requestQueue.isEmpty()) {
        m_state = State::Ready;
        return;
    }

    m_currentRequest = m_requestQueue.dequeue();

    if(m_currentRequest.device->isDFU()) {
        processCurrentRequest();
    } else {
        m_state = State::WaitingForDFU;
        m_currentRequest.device->detach();
    }
}

void FirmwareDownloader::enqueueRequest(const Request &req)
{
    req.device->setStatusMessage(tr("Pending"));

    m_requestQueue.enqueue(req);

    if(m_state == State::Ready) {
        processQueue();
    }
}

void FirmwareDownloader::processCurrentRequest()
{
    m_state = State::ExecuteRequest;

    auto *device = m_currentRequest.device;
    device->setStatusMessage(tr("Preparing"));

    auto *watcher = new QFutureWatcher<bool>(this);

    connect(watcher, &QFutureWatcherBase::finished, watcher, &QObject::deleteLater);
    connect(watcher, &QFutureWatcherBase::finished, m_currentRequest.file, &QObject::deleteLater);
    connect(watcher, &QFutureWatcherBase::finished, this, &FirmwareDownloader::processQueue);
    connect(watcher, &QFutureWatcherBase::finished, this, [=]() {
        device->setStatusMessage(watcher->result() ? tr("Finished") : tr("Error"));
    });

    watcher->setFuture(QtConcurrent::run(m_currentRequest.device, &Flipper::FlipperZero::downloadFirmware, m_currentRequest.file));
}

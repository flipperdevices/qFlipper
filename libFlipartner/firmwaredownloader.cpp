#include "firmwaredownloader.h"

#include <QUrl>
#include <QFile>
#include <QFutureWatcher>
#include <QSerialPortInfo>
#include <QtConcurrent/QtConcurrentRun>

#include <QDebug>

#include "flipperzero.h"

using namespace Flipper;

FirmwareDownloader::FirmwareDownloader(QObject *parent):
    QObject(parent),
    m_state(State::Ready)
{}

void FirmwareDownloader::downloadLocalFile(Zero *device, const QString &filePath)
{
    device->setStatusMessage(tr("Pending"));

    const auto localUrl = QUrl(filePath).toLocalFile();
    auto *file = new QFile(localUrl, this);

    m_requestQueue.enqueue({
        device,
        file
    });

    if(m_state == State::Ready) {
        processQueue();
    }
}

void FirmwareDownloader::onDeviceConnected(Zero *device)
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

void FirmwareDownloader::processCurrentRequest()
{
    m_state = State::ExecuteRequest;

    m_currentRequest.device->setStatusMessage(tr("Preparing"));

    auto *watcher = new QFutureWatcher<bool>(this);

    connect(watcher, &QFutureWatcherBase::finished, this, &FirmwareDownloader::processQueue);
    connect(watcher, &QFutureWatcherBase::finished, m_currentRequest.file, &QObject::deleteLater);
    connect(watcher, &QFutureWatcherBase::finished, watcher, &QObject::deleteLater);

    watcher->setFuture(QtConcurrent::run(m_currentRequest.device, &Flipper::Zero::download, m_currentRequest.file));
}

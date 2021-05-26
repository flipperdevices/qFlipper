#include "firmwareupdater.h"

#include <QUrl>
#include <QDebug>
#include <QFile>
#include <QThreadPool>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "serialhelper.h"
#include "firmwaredownloadtask.h"

FirmwareUpdater::FirmwareUpdater(QObject *parent):
    QObject(parent),
    m_state(State::Ready)
{}

void FirmwareUpdater::requestLocalUpdate(const FlipperInfo &info, const QString &filePath)
{
    Request req = {
        info,
        new QFile(QUrl(filePath).toLocalFile())
    };

    req.info.status.message = tr("Pending");
    emit deviceStatusChanged(req.info);

    m_requestQueue.enqueue(req);

    if(m_state == State::Ready) {
        processQueue();
    }
}

void FirmwareUpdater::onDeviceConnected(const FlipperInfo &info)
{
    if(m_state != State::WaitingForDFU) {
        return;
    }

    m_currentRequest.info = info;
    m_currentRequest.info.status.message = tr("Preparing");
    emit deviceStatusChanged(m_currentRequest.info);

    m_state = State::ExecuteRequest;
    downloadFirmware(info, m_currentRequest.file);
}

void FirmwareUpdater::processQueue()
{
    if(m_requestQueue.isEmpty()) {
        m_state = State::Ready;
        return;
    }

    m_currentRequest = m_requestQueue.dequeue();

    if(m_currentRequest.info.isDFU()) {
        m_state = State::ExecuteRequest;
        downloadFirmware(m_currentRequest.info, m_currentRequest.file);
    } else {
        m_state = State::WaitingForDFU;
        resetToDFU(m_currentRequest.info);
    }
}

void FirmwareUpdater::downloadFirmware(const FlipperInfo &info, QIODevice *file)
{
    auto *task = new FirmwareDownloadTask(info, file);

    connect(task, &FirmwareDownloadTask::statusChanged, this, &FirmwareUpdater::deviceStatusChanged);
    connect(task, &FirmwareDownloadTask::finished, this, &FirmwareUpdater::processQueue);
    connect(task, &FirmwareDownloadTask::finished, file, &QObject::deleteLater);

    QThreadPool::globalInstance()->start(task);
}

void FirmwareUpdater::resetToDFU(const FlipperInfo &info)
{
    const auto portInfo = SerialHelper::findSerialPort(info);

    if(portInfo.isNull()) {
        // TODO: Error handling - we might not find the port
        return;
    }

    QSerialPort port(portInfo);

    if(port.open(QIODevice::ReadWrite)) {
        port.write("dfu\r");
        port.flush();
        port.close();

    } else {
        // TODO: Error handling
        qCritical() << "Failed to open serial port" << portInfo.portName();
    }

}

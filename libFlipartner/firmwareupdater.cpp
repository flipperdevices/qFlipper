#include "firmwareupdater.h"

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

void FirmwareUpdater::requestLocalFlash(const QString &serialNumber, const QString &filePath)
{
    // Enqueue the request
    Request req = {
        serialNumber,
        new QFile(filePath) // TODO: free the memory
    };

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

    m_state = State::ExecuteRequest;
    downloadFirmware(info, m_currentRequest.file);
}

void FirmwareUpdater::onDeviceFound(const FlipperInfo &info)
{
    if(m_state != State::WaitingForInfo) {
        return;
    }

    // Determine if the device is in DFU mode
    if(info.params.productID == 0xdf11) {
        m_state = State::ExecuteRequest;
        downloadFirmware(info, m_currentRequest.file);
    } else {
        // If the device is not in DFU mode, reset it and wait for DFU in onDeviceConnected()
        m_state = State::WaitingForDFU;
        resetToDFU(info);
    }
}

void FirmwareUpdater::processQueue()
{
    if(m_requestQueue.isEmpty()) {
        m_state = State::Ready;
        return;
    }

    m_state = State::WaitingForInfo;
    m_currentRequest = m_requestQueue.dequeue();
    emit deviceInfoRequested(m_currentRequest.serialNumber);
}

void FirmwareUpdater::onDownloadFinished()
{
    qDebug() << "Download finished!";
    processQueue();
}

void FirmwareUpdater::downloadFirmware(const FlipperInfo &info, QIODevice *file)
{
    qDebug() << "Downloading firmware..." << info.params.serialNumber;
    auto *task = new FirmwareDownloadTask(info, file);

    connect(task, &FirmwareDownloadTask::finished, this, &FirmwareUpdater::onDownloadFinished);

    QThreadPool::globalInstance()->start(task);
}

void FirmwareUpdater::resetToDFU(const FlipperInfo &info)
{
    const auto portInfo = SerialHelper::findSerialPort(info);

    if(portInfo.isNull()) {
        // TODO: Error handling - we might not find the port
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

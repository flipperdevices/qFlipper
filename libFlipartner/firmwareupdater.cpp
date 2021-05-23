#include "firmwareupdater.h"

#include <QDebug>
#include <QFile>
#include <QSerialPort>
#include <QSerialPortInfo>

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

    downloadFirmware(info, m_currentRequest.file);
}

void FirmwareUpdater::onDeviceFound(const FlipperInfo &info)
{
    if(m_state != State::WaitingForInfo) {
        return;
    }

    // Determine if the device is in DFU mode
    if(info.params.productID == 0xdf11) {
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

void FirmwareUpdater::downloadFirmware(const FlipperInfo &info, QIODevice *file)
{
    qDebug() << "Downloading firmware..." << info.params.serialNumber;
}

void FirmwareUpdater::resetToDFU(const FlipperInfo &info)
{
    const auto portInfos = QSerialPortInfo::availablePorts();
    for(const auto &portInfo : portInfos) {
        if(info.params.serialNumber == portInfo.serialNumber()) {
            QSerialPort port(portInfo);

            if(port.open(QIODevice::ReadWrite)) {
                port.write("dfu\r");
                port.flush();
                port.close();

            } else {
                qCritical() << "Failed to open serial port" << portInfo.portName();
            }

            break;
        }
    }
}

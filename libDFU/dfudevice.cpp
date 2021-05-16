#include "dfudevice.h"

#include <QDebug>
#include <QThread>
#include <QtEndian>
#include <QByteArray>

static constexpr const char *dbgLabel = "DFUDevice:";

DFUDevice::DFUDevice(const USBDeviceInfo &info, QObject *parent):
    USBDevice(info, parent)
{}

bool DFUDevice::beginTransaction(int alt)
{
    Q_UNUSED(alt)
    return open() && claimInterface(0);
}

bool DFUDevice::endTransaction()
{
    const auto res = releaseInterface(0);
    close();
    return res;
}

bool DFUDevice::abortToIdle()
{
    if(!controlTransfer(ENDPOINT_OUT | REQUEST_TYPE_CLASS | RECIPIENT_INTERFACE, DFU_ABORT, 0, 0, QByteArray())) {
        qCritical() << dbgLabel << "Unable to issue abort request";
        return false;
    }

    const auto status = getStatus();
    const auto res = (status.error == Status::OK) && (status.state == Status::DFU_IDLE);

    if(!res) {
        qCritical() << dbgLabel << "Unable to reset device to idle state";
    } else {
        QThread::msleep(status.timeout);
    }

    return res;
}

bool DFUDevice::clearStatus()
{
    return controlTransfer(ENDPOINT_OUT | REQUEST_TYPE_CLASS | RECIPIENT_INTERFACE, DFU_CLRSTATUS, 0, 0, QByteArray());
}

DFUDevice::Status DFUDevice::getStatus()
{
    Status ret;

    const auto STATUS_LENGTH = 6;
    const auto buf = controlTransfer(ENDPOINT_IN | REQUEST_TYPE_CLASS | RECIPIENT_INTERFACE, DFU_GETSTATUS, 0, 0, STATUS_LENGTH);

    if(buf.size() != STATUS_LENGTH) {
        qCritical() << dbgLabel << "Unable to get device status";
        return ret;
    }

    ret.error = (Status::Error)buf[0];
    ret.state = (Status::State)buf[4];

    ret.timeout = ((uint32_t)buf[3] << 16) |
                  ((uint32_t)buf[2] << 8)  |
                  ((uint32_t)buf[1]);

    ret.istring = buf[5];

    return ret;
}

// NOTE: The following code is purely for testing purposes
bool DFUDevice::download(QIODevice &file, uint32_t addr)
{
    Q_UNUSED(file)
    Q_UNUSED(addr)

//    if(!clearStatus()) {
//        return false;
//    }

//    if(!setAddressPointer(addr)) {
//        qCritical() << dbgLabel << "Failed to set address pointer";
//        return false;
//    }

    // !!! Warning! This code doesn't work properly -- it bricks devices!
    // (but it means it does at least SOMETHING) :D

//    uint16_t transaction = 2;

//    while(!file.atEnd()) {
//        auto buf = file.read(128);

//        if(!controlTransfer(0x21, 1, transaction++, 0, buf)) {
//            qCritical() << dbgLabel << "Unable to perform control transfer";
//            return false;
//        }

//        do {
//            const auto status = getStatus();

//            if(!status.isValid()) {
//                releaseInterface(0);
//                return false;
//            } else if(status.state == 5) {
//                break;
//            }

//            qDebug() << "Status:" << status.status;
//            qDebug() << "Poll Timeout:" << status.timeout;
//            qDebug() << "State:" << status.state;

//            QThread::msleep(status.timeout + 10);

//        } while(1);
//    }

//    controlTransfer(0x21, 1, transaction, 0, QByteArray());

    return true;
}

bool DFUDevice::upload(QIODevice &file, uint32_t addr, size_t maxSize)
{
    const auto status = getStatus();

    if(status.error != Status::OK) {
        qInfo() << dbgLabel << "Device is in error state, resetting...";

        if(!clearStatus()) {
            qCritical() << dbgLabel << "Failed to clear device status";
            return false;
        }

    } else if(status.state != Status::DFU_IDLE) {
        qInfo() << dbgLabel << "Device is not idle, resetting...";

        if(!abortToIdle()) {
            qCritical() << dbgLabel << "Failed to abort to idle";
            return false;
        }
    }

    if(!(setAddressPointer(addr) && abortToIdle())) {
        qCritical() << dbgLabel << "Failed to set address pointer";
        return false;
    }

    uint16_t transaction = 2;

    for(size_t totalSize = 0; totalSize < maxSize; ) {
        const auto MAX_BUF_SIZE = 2048; // TODO: Determine this size from configuration descriptor

        const auto transactionSize = qMin<size_t>(MAX_BUF_SIZE, maxSize - totalSize);
        const auto buf = controlTransfer(ENDPOINT_IN | REQUEST_TYPE_CLASS | RECIPIENT_INTERFACE, DFU_UPLOAD, transaction++, 0, transactionSize);

        if((size_t)buf.size() != transactionSize) {
            qCritical() << dbgLabel << "Unable to perform upload control transfer";
            return false;
        }

        const auto bytesWritten = file.write(buf);

        if(bytesWritten < 0) {
            qCritical() << dbgLabel << "Failed to write to output device";
            return false;
        }

        totalSize += bytesWritten;

        qDebug() << dbgLabel << "Bytes uploaded:" << totalSize << totalSize * 100 / maxSize << "%";
    }

    return true;
}

bool DFUDevice::setAddressPointer(uint32_t addr)
{
    const auto requestData = QByteArray(1, 0x21) + QByteArray::fromRawData((char*)&addr, sizeof(uint32_t));

    if(!controlTransfer(ENDPOINT_OUT | REQUEST_TYPE_CLASS | RECIPIENT_INTERFACE, DFU_DNLOAD, 0, 0, requestData)) {
        return false;
    }

    Status status;

    do {
        status = getStatus();

        if(status.error == Status::OK) {
            QThread::msleep(status.timeout);
        } else {
            return false;
        }

    } while(status.state == Status::DFU_DNBUSY);

    return true;
}

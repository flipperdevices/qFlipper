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

bool DFUDevice::clearStatus()
{
    Status status;
    bool res;

    auto attemptsLeft = 10;

    do {
        res = controlTransfer(ENDPOINT_OUT | REQUEST_TYPE_CLASS | RECIPIENT_INTERFACE, DFU_CLRSTATUS, 0, 0, QByteArray());

        if(!res) {
            qCritical() << dbgLabel << "Unable to clear device status";
            break;

        } else if(!attemptsLeft--) {
            qCritical() << dbgLabel << "Exceeded attempt count to clear device status";
            return false;

        } else {}

        status = getStatus();

    } while(status.state != Status::DFU_IDLE);

    return res;
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

    if(!clearStatus()) {
        return false;
    }

    if(!setAddressPointer(addr)) {
        qCritical() << dbgLabel << "Failed to set address pointer";
        return false;
    }

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

bool DFUDevice::upload(QIODevice &file, uint32_t addr, size_t len)
{
    if(!clearStatus()) {
        return false;
    }

    if(!setAddressPointer(addr)) {
        qCritical() << dbgLabel << "Failed to set address pointer";
        return false;
    }

    if(!clearStatus()) {
        return false;
    }

    uint16_t transaction = 2;

    for(size_t totalSize = 0; totalSize < len; ) {
        const auto BUFSIZE = 256;
        const auto buf = controlTransfer(ENDPOINT_IN | REQUEST_TYPE_CLASS | RECIPIENT_INTERFACE, DFU_UPLOAD, transaction++, 0, BUFSIZE);

        if(buf.size() != BUFSIZE) {
            qCritical() << dbgLabel << "Unable to perform upload control transfer";
            return false;
        }

        totalSize += file.write(buf);

        qDebug() << dbgLabel << "Bytes uploaded:" << totalSize;
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
        QThread::msleep(status.timeout);

    } while(status.state == Status::DFU_DNBUSY);

    return true;
}

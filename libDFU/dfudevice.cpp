#include "dfudevice.h"

#include <QDebug>
#include <QThread>
#include <QByteArray>

static constexpr const char *dbgLabel = "DFUDevice:";

DFUDevice::DFUDevice(const USBDeviceInfo &info, QObject *parent):
    USBDevice(info, parent)
{}

bool DFUDevice::clearStatus()
{
    const auto res = controlTransfer(0x21, 4, 0, 0, QByteArray());

    if(!res) {
        qCritical() << dbgLabel << "Unable to clear device status";
    }

    return res;
}

DFUDevice::Status DFUDevice::getStatus()
{
    Status ret;

    const auto STATUS_LENGTH = 6;
    const auto buf = controlTransfer(0xa1, 3, 0, 0, STATUS_LENGTH);

    if(buf.size() != STATUS_LENGTH) {
        qCritical() << dbgLabel << "Unable to get device status";
        return ret;
    }

    ret.status = buf[0];
    ret.state = buf[4];
    ret.istring = buf[5];

    ret.timeout = ((uint32_t)buf[3] << 16) |
                  ((uint32_t)buf[2] << 8)  |
                  ((uint32_t)buf[1]);

    return ret;
}

// NOTE: The following code is purely for testing purposes
bool DFUDevice::download(QIODevice &file, int alt)
{
    Q_UNUSED(file)
    Q_UNUSED(alt)

    claimInterface(0);

    const auto status = getStatus();

    qDebug() << "Status:" << status.status;
    qDebug() << "Poll Timeout:" << status.timeout;
    qDebug() << "State:" << status.state;

    // !!! Warning! This code doesn't work properly -- it bricks devices!
    // (but it means it does at least SOMETHING) :D

//    uint16_t transaction = 0;

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

    releaseInterface(0);

    return true;
}

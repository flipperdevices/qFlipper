#include "dfudevice.h"

#include <QDebug>
#include <QThread>
#include <QByteArray>

// Don't forget!
// TODO: replace if() error check blocks with a macro/function

static constexpr const char *dbgLabel = "DFUDevice:";

DFUDevice::DFUDevice(const USBDeviceInfo &info, QObject *parent):
    USBDevice(info, parent)
{}

bool DFUDevice::beginTransaction(uint8_t alt)
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

bool DFUDevice::erase(uint32_t addr, size_t maxSize)
{
    // NOTE: This code only erases the first page.
    // TODO: Get and utilise flash memory layout.

    Q_UNUSED(maxSize)

    if(!prepare()) {
        qCritical() << "Failed to prepare the device";
        return false;
    }

    const auto buf = QByteArray(1, 0x41) + QByteArray((const char*)&addr, sizeof(uint32_t));

    if(!controlTransfer(ENDPOINT_OUT | REQUEST_TYPE_CLASS | RECIPIENT_INTERFACE, DFU_DNLOAD, 0, 0, buf)) {
        qCritical() << dbgLabel << "Failed to perform DFU_DNLOAD transfer";
        return false;
    }

    StatusType status;

    do {
        status = getStatus();

        if(status.bStatus != StatusType::OK) {
            qCritical() << dbgLabel << "An error has occured during erase phase";
            return false;
        } else{
            QThread::msleep(status.bwPollTimeout);
        }

    } while(status.bState == StatusType::DFU_DNBUSY);

    qInfo() << dbgLabel << "Erase done.";

    return true;
}

bool DFUDevice::download(QIODevice &file, uint32_t addr)
{
    if(!prepare()) {
        qCritical() << dbgLabel << "Failed to prepare the device";
        return false;
    }

    qInfo() << dbgLabel << "Erasing the memory...";

    if(!erase(addr, file.size())) {
        qCritical() << dbgLabel << "Failed to erase the memory";
        return false;
    }

    if(!setAddressPointer(addr)) {
        qCritical() << dbgLabel << "Failed to set address pointer";
        return false;
    }

    const auto extra = extraInterfaceDescriptor();

    if(extra.size() < 9) {
        qCritical() << dbgLabel << "No functional DFU descriptor";
        return false;
    }

    const auto maxTransferSize = *((uint16_t*)(extra.data() + 5));
    qInfo() << dbgLabel << "Device reported transfer size:" << maxTransferSize;

    StatusType status;

    for(size_t totalSize = 0, transaction = 2; !file.atEnd(); ++transaction) {
        const auto buf = file.read(maxTransferSize);

        if(!controlTransfer(ENDPOINT_OUT | REQUEST_TYPE_CLASS | RECIPIENT_INTERFACE, DFU_DNLOAD, transaction, 0, buf)) {
            qCritical() << dbgLabel << "Failed to perform DFU_DNLOAD transfer";
            return false;
        }

        do {
            status = getStatus();

            if(status.bStatus != StatusType::OK) {
                qCritical() << dbgLabel << "An error has occured during download phase";
                return false;
            } else {
                QThread::msleep(status.bwPollTimeout);
            }

        } while(status.bState != StatusType::DFU_DNLOAD_IDLE);

        totalSize += buf.size();

        qDebug() << dbgLabel << "Bytes downloaded:" << totalSize << totalSize * 100 / file.size() << "%";
    }

    if(!controlTransfer(ENDPOINT_OUT | REQUEST_TYPE_CLASS | RECIPIENT_INTERFACE, DFU_DNLOAD, 0, 0, QByteArray())) {
        qCritical() << dbgLabel << "Failed to perform final DFU_DNLOAD transfer";
        return false;
    }

    do {
        status = getStatus();

        if(status.bStatus != StatusType::OK) {
            qCritical() << dbgLabel << "An error has occured during manifestation phase";
            return false;

        } else {
            QThread::msleep(status.bwPollTimeout);
        }

    } while(status.bState != StatusType::DFU_MANIFEST);

    qInfo() << dbgLabel << "Download has finished.";

    return true;
}

bool DFUDevice::upload(QIODevice &file, uint32_t addr, size_t maxSize)
{
    if(!prepare()) {
        qCritical() << "Failed to prepare the device";
        return false;
    }

    if(!(setAddressPointer(addr) && abort())) {
        qCritical() << dbgLabel << "Failed to set address pointer";
        return false;
    }

    const auto extra = extraInterfaceDescriptor();

    if(extra.size() < 9) {
        qCritical() << dbgLabel << "No functional DFU descriptor";
        return false;
    }

    const auto maxTransferSize = *((uint16_t*)(extra.data() + 5));
    qInfo() << dbgLabel << "Device reported transfer size:" << maxTransferSize;

    for(size_t totalSize = 0, transaction = 2; totalSize < maxSize; ++transaction) {

        const auto transferSize = qMin<size_t>(maxTransferSize, maxSize - totalSize);
        const auto buf = controlTransfer(ENDPOINT_IN | REQUEST_TYPE_CLASS | RECIPIENT_INTERFACE, DFU_UPLOAD, transaction, 0, transferSize);

        const auto bytesWritten = file.write(buf);

        if(bytesWritten < 0) {
            qCritical() << dbgLabel << "Failed to write to output device";
            return false;
        }

        totalSize += bytesWritten;

        qDebug() << dbgLabel << "Bytes uploaded:" << totalSize << totalSize * 100 / maxSize << "%";

        // TODO: Better error checks
        // Correctly process the end of memory condition?
        if((size_t)buf.size() < transferSize) {
            qInfo() << dbgLabel << "End of transmission.";
            break;
        }
    }

    qInfo() << dbgLabel << "Upload has finished.";

    return true;
}

bool DFUDevice::setAddressPointer(uint32_t addr)
{
    const auto requestData = QByteArray(1, 0x21) + QByteArray::fromRawData((char*)&addr, sizeof(uint32_t));

    if(!controlTransfer(ENDPOINT_OUT | REQUEST_TYPE_CLASS | RECIPIENT_INTERFACE, DFU_DNLOAD, 0, 0, requestData)) {
        return false;
    }

    StatusType status;

    do {
        status = getStatus();

        if(status.bStatus == StatusType::OK) {
            QThread::msleep(status.bwPollTimeout);
        } else {
            return false;
        }

    } while(status.bState == StatusType::DFU_DNBUSY);

    return true;
}

bool DFUDevice::abort()
{
    if(!controlTransfer(ENDPOINT_OUT | REQUEST_TYPE_CLASS | RECIPIENT_INTERFACE, DFU_ABORT, 0, 0, QByteArray())) {
        qCritical() << dbgLabel << "Unable to issue abort request";
        return false;
    }

    const auto status = getStatus();
    const auto res = (status.bStatus == StatusType::OK) && (status.bState == StatusType::DFU_IDLE);

    if(!res) {
        qCritical() << dbgLabel << "Unable to reset device to idle state";
    } else {
        QThread::msleep(status.bwPollTimeout);
    }

    return res;
}

bool DFUDevice::clearStatus()
{
    return controlTransfer(ENDPOINT_OUT | REQUEST_TYPE_CLASS | RECIPIENT_INTERFACE, DFU_CLRSTATUS, 0, 0, QByteArray());
}

DFUDevice::StatusType DFUDevice::getStatus()
{
    StatusType ret;

    const auto STATUS_LENGTH = 6;
    const auto buf = controlTransfer(ENDPOINT_IN | REQUEST_TYPE_CLASS | RECIPIENT_INTERFACE, DFU_GETSTATUS, 0, 0, STATUS_LENGTH);

    if(buf.size() != STATUS_LENGTH) {
        qCritical() << dbgLabel << "Unable to get device status";
        return ret;
    }

    ret.bStatus = (StatusType::Status)buf[0];
    ret.bState = (StatusType::State)buf[4];

    ret.bwPollTimeout = ((uint32_t)buf[3] << 16) |
                  ((uint32_t)buf[2] << 8)  |
                  ((uint32_t)buf[1]);

    ret.iString = buf[5];

    return ret;
}

bool DFUDevice::prepare()
{
    const auto status = getStatus();

    if(status.bStatus != StatusType::OK) {
        qInfo() << dbgLabel << "Device is in error state, resetting...";

        if(!clearStatus()) {
            qCritical() << dbgLabel << "Failed to clear device status";
            return false;
        }

    } else if(status.bState != StatusType::DFU_IDLE) {
        qInfo() << dbgLabel << "Device is not idle, resetting...";

        if(!abort()) {
            qCritical() << dbgLabel << "Failed to abort to idle";
            return false;
        }
    }

    return true;
}

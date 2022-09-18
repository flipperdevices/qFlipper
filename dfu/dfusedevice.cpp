#include "dfusedevice.h"

#include <cmath>

#include <QThread>
#include <QBuffer>
#include <QByteArray>

#include "debug.h"
#include "dfumemorylayout.h"

#define REQUEST_OUT (USBRequest::ENDPOINT_OUT | USBRequest::REQUEST_TYPE_CLASS | USBRequest::RECIPIENT_INTERFACE)
#define REQUEST_IN (USBRequest::ENDPOINT_IN | USBRequest::REQUEST_TYPE_CLASS | USBRequest::RECIPIENT_INTERFACE)

#define DFU_DESCRIPTOR_LENGTH 9
#define DFU_DESCRIPTOR_TYPE 0x21

DfuseDevice::DfuseDevice(const USBDeviceInfo &info, QObject *parent):
    USBDevice(info, parent)
{}

bool DfuseDevice::beginTransaction()
{
    return open() && claimInterface(0);
}

bool DfuseDevice::endTransaction()
{
    const auto res = releaseInterface(0);
    close();
    return res;
}

uint32_t DfuseDevice::partitionOrigin(uint8_t alt)
{
    const auto desc = stringInterfaceDescriptor(alt);
    const auto layout = DFUMemoryLayout::fromStringDescriptor(desc);
    return layout.address();
}

bool DfuseDevice::erase(uint32_t addr, size_t maxSize)
{
    check_return_bool(prepare(), "Failed to prepare the device");

    const auto layout = DFUMemoryLayout::fromStringDescriptor(stringInterfaceDescriptor(0));
    const auto pageAddresses = layout.pageAddresses(addr, addr + (uint32_t)maxSize);

    check_return_bool(!pageAddresses.isEmpty(), "Address list is empty");

    size_t prevProgress = std::numeric_limits<size_t>::max();

    for(const auto pageAddress: pageAddresses) {
        check_return_bool(erasePage(pageAddress), "Failed to erase page");
        const auto progress = (pageAddress - addr) * 100.0 / maxSize;
        emit progressChanged(Operation::Erase, progress);

        if(floor(progress) != prevProgress) {
            prevProgress = progress;
            debug_msg(QString("Erasing memory: %1%").arg(prevProgress));
        }

    }

    debug_msg("Erase done.");

    return true;
}

bool DfuseDevice::download(DfuseFile *file)
{
    check_return_bool(file->isValid(), "DfuSe file is not valid");
    // TODO: check for vendor, device, etc.

    // Erase the required memory first
    for(auto &img : file->images()) {
        for(auto &elem : img.elements) {
            check_return_bool(erase(elem.dwElementAddress, elem.dwElementSize), "Failed to erase the memory");
        }
    }

    for(auto &img : file->images()) {
        for(auto &elem : img.elements) {
            QBuffer buf(&elem.data);
            buf.open(QIODevice::ReadOnly);
            check_return_bool(download(&buf, elem.dwElementAddress, img.prefix.bAlternateSetting), "Failed to download element");
        }
    }

    return true;
}

bool DfuseDevice::download(const QByteArray &data)
{
    check_return_bool(setInterfaceAltSetting(0, 0), "Failed to set interface alternate setting");
    check_return_bool(prepare(), "Failed to prepare the device");

    check_return_bool(controlTransfer(REQUEST_OUT, DFU_DNLOAD, 0, 0, data), "Failed to perform raw download request");

    StatusType status;

    do {
        status = getStatus();
        check_return_bool(status.bStatus == StatusType::OK, "Failed to raw download a buffer");
        QThread::msleep(status.bwPollTimeout);

    } while(status.bState == StatusType::DFU_DNBUSY);

    return true;
}

bool DfuseDevice::download(QIODevice *file, uint32_t addr, uint8_t alt)
{
    check_return_bool(setInterfaceAltSetting(0, alt), "Failed to set interface alternate setting");
    check_return_bool(prepare(), "Failed to prepare the device");
    check_return_bool(setAddressPointer(addr), "Failed to set address pointer");

    const auto extra = extraInterfaceDescriptor(0, DFU_DESCRIPTOR_TYPE, DFU_DESCRIPTOR_LENGTH);
    check_return_bool(extra.size() >= DFU_DESCRIPTOR_LENGTH, "No functional DFU descriptor");

    const auto maxTransferSize = *((uint16_t*)(extra.data() + 5));
    debug_msg(QString("Device reported transfer size: %1").arg(maxTransferSize));

    StatusType status;

    for(size_t totalSize = 0, transaction = 2, prevProgress = std::numeric_limits<size_t>::max(); !file->atEnd(); ++transaction) {
        const auto buf = file->read(maxTransferSize);
        check_return_bool(controlTransfer(REQUEST_OUT, DFU_DNLOAD, (uint16_t)transaction, 0, buf), "Failed to perform DFU_DNLOAD transfer");

        do {
            status = getStatus();

            check_return_bool(status.bStatus == StatusType::OK, "An error has occurred during download phase");
            QThread::msleep(status.bwPollTimeout);

        } while(status.bState != StatusType::DFU_DNLOAD_IDLE);

        totalSize += buf.size();

        const auto progress = totalSize * 100.0 / file->size();
        emit progressChanged(Operation::Download, progress);

        if(floor(progress) != prevProgress) {
            prevProgress = progress;
            debug_msg(QString("Bytes downloaded: %1 %2%").arg(totalSize).arg(prevProgress));
        }
    }

    debug_msg("Download has finished.");

    return true;
}

bool DfuseDevice::upload(QIODevice *file, uint32_t addr, size_t maxSize, uint8_t alt)
{
    check_return_bool(setInterfaceAltSetting(0, alt), "Failed to set interface alternate setting");
    check_return_bool(prepare() && abort(), "Failed to prepare the device");
    check_return_bool(setAddressPointer(addr), "Failed to set address pointer");
    abort();

    const auto extra = extraInterfaceDescriptor(0, DFU_DESCRIPTOR_TYPE, DFU_DESCRIPTOR_LENGTH);
    check_return_bool(extra.size() >= DFU_DESCRIPTOR_LENGTH, "No functional DFU descriptor");

    const auto maxTransferSize = *((uint16_t*)(extra.data() + 5));

    debug_msg(QString("Device reported transfer size: %1").arg(maxTransferSize));

    for(size_t totalSize = 0, transaction = 2, prevProgress = std::numeric_limits<size_t>::max(); totalSize < maxSize; ++transaction) {

        const auto transferSize = qMin<size_t>(maxTransferSize, maxSize - totalSize);
        const auto buf = controlTransfer(REQUEST_IN, DFU_UPLOAD, (uint16_t)transaction, 0, (uint16_t)transferSize);

        const auto bytesWritten = file->write(buf);

        check_return_bool(bytesWritten >= 0, "Failed to write to output device");

        totalSize += bytesWritten;

        const auto progress = totalSize * 100.0 / maxSize;
        emit progressChanged(Operation::Upload, progress);

        if(floor(progress) != prevProgress) {
            prevProgress = progress;
            debug_msg(QString("Bytes uploaded: %1 %2%").arg(totalSize).arg(prevProgress));
        }

        // TODO: Better error checks
        // Correctly process the end of memory condition?
        if((size_t)buf.size() < transferSize) {
            debug_msg("End of transmission.");
            break;
        }
    }

    debug_msg("Upload has finished.");

    return true;
}

bool DfuseDevice::leave()
{
    check_return_bool(setInterfaceAltSetting(0, 0), "Failed to set interface alternate setting");
    check_return_bool(prepare() && abort(), "Failed to prepare the device");

    setAddressPointer(0x080FFFFFUL);
    check_return_bool(controlTransfer(REQUEST_OUT, DFU_DNLOAD, 0, 0, QByteArray()), "Failed to perform final DFU_DNLOAD transfer");

    begin_ignore_block();
    getStatus(); // It will return an error on WB55 anyway
    end_ignore_block();

    return true;
}

bool DfuseDevice::setAddressPointer(uint32_t addr)
{
    const auto requestData = QByteArray(1, 0x21) + QByteArray::fromRawData((char*)&addr, sizeof(uint32_t));
    check_return_bool(controlTransfer(REQUEST_OUT, DFU_DNLOAD, 0, 0, requestData), "Failed to perform set address request");

    StatusType status;

    do {
        status = getStatus();
        check_return_bool(status.bStatus == StatusType::OK, "Failed to set address pointer");
        QThread::msleep(status.bwPollTimeout);

    } while(status.bState == StatusType::DFU_DNBUSY);

    return true;
}

bool DfuseDevice::erasePage(uint32_t addr)
{
    const auto buf = QByteArray(1, 0x41) + QByteArray((const char*)&addr, sizeof(uint32_t));
    check_return_bool(controlTransfer(REQUEST_OUT, DFU_DNLOAD, 0, 0, buf), "Failed to perform DFU_DNLOAD transfer");

    StatusType status;

    do {
        status = getStatus();
        check_return_bool(status.bStatus == StatusType::OK, "An error has occurred during erase phase");
        QThread::msleep(status.bwPollTimeout);

    } while(status.bState == StatusType::DFU_DNBUSY);

    return true;
}

bool DfuseDevice::abort()
{
    check_return_bool(controlTransfer(REQUEST_OUT, DFU_ABORT, 0, 0, QByteArray()), "Unable to issue abort request");

    const auto status = getStatus();
    const auto res = (status.bStatus == StatusType::OK) && (status.bState == StatusType::DFU_IDLE);

    check_return_bool(res, "Unable to reset device to idle state");
    QThread::msleep(status.bwPollTimeout);

    return res;
}

bool DfuseDevice::clearStatus()
{
    return controlTransfer(REQUEST_OUT, DFU_CLRSTATUS, 0, 0, QByteArray());
}

DfuseDevice::StatusType DfuseDevice::getStatus()
{
    StatusType ret;

    const auto STATUS_LENGTH = 6;
    const auto buf = controlTransfer(REQUEST_IN, DFU_GETSTATUS, 0, 0, STATUS_LENGTH);

    check_return_val(buf.size() == STATUS_LENGTH, "Unable to get device status", ret);

    ret.bStatus = (StatusType::Status)buf[0];
    ret.bState = (StatusType::State)buf[4];

    ret.bwPollTimeout = ((uint32_t)buf[3] << 16) |
                        ((uint32_t)buf[2] << 8)  |
                        ((uint32_t)buf[1]);

    ret.iString = buf[5];

    return ret;
}

bool DfuseDevice::prepare()
{
    const auto status = getStatus();

    if(status.bStatus != StatusType::OK) {
        debug_msg("Device is in error state, resetting...");
        check_return_bool(clearStatus(), "Failed to clear device status");

    } else if(status.bState != StatusType::DFU_IDLE) {
        debug_msg("Device is not idle, resetting...");
        check_return_bool(abort(), "Failed to abort to idle");
    }

    return true;
}

#include "usbdevice.h"

#include <libusb.h>
#include <QThread>

#include "debug.h"

#define RETRY_COUNT 25
#define RETRY_INTERVAL_MS 50

struct USBDevice::USBDevicePrivate {
    libusb_device *libusbDevice = nullptr;
    libusb_device_handle *libusbDeviceHandle = nullptr;
};

USBDevice::USBDevice(const USBDeviceInfo &info, QObject *parent):
    QObject(parent),
    m_p(new USBDevicePrivate)
{
    m_p->libusbDevice = (libusb_device*)info.backendData().value<void*>();
}

USBDevice::~USBDevice()
{
    // TODO: Find out how to close it safely
//  close();
    delete m_p;
}

bool USBDevice::open()
{
    if(!m_p->libusbDeviceHandle) {
        const auto err = libusb_open(m_p->libusbDevice, &(m_p->libusbDeviceHandle));
        check_continue(!err, QString("Unable to open device: %1").arg(libusb_error_name(err)));
    }

    return m_p->libusbDeviceHandle;
}

void USBDevice::close()
{
    if(!m_p->libusbDeviceHandle) {
        return;
    }

    libusb_close(m_p->libusbDeviceHandle);
    m_p->libusbDeviceHandle = nullptr;
}

bool USBDevice::claimInterface(int interfaceNum)
{
    int retryCount = RETRY_COUNT, err;

    do {
        if(!(err = libusb_claim_interface(m_p->libusbDeviceHandle, interfaceNum))) { break; }
        QThread::msleep(RETRY_INTERVAL_MS);
    } while(--retryCount);

    check_continue(!err, QString("Failed to claim interface: %1").arg(libusb_error_name(err)));
    return !err;
}

bool USBDevice::releaseInterface(int interfaceNum)
{
    const auto err = libusb_release_interface(m_p->libusbDeviceHandle, interfaceNum);
    check_continue(!err, QString("Failed to release interface: %1").arg(libusb_error_name(err)));
    return !err;
}

bool USBDevice::setInterfaceAltSetting(int interfaceNum, uint8_t alt)
{
    int retryCount = RETRY_COUNT, err;

    do {
        if(!(err = libusb_set_interface_alt_setting(m_p->libusbDeviceHandle, interfaceNum, alt))) { break; }
        QThread::msleep(RETRY_INTERVAL_MS);
    } while(--retryCount);

    check_continue(!err, QString("Failed to set alternate setting: %1").arg(libusb_error_name(err)));
    return !err;
}

bool USBDevice::controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, const QByteArray &buf)
{
    int retryCount = RETRY_COUNT, res;

    do {
        auto *data = buf.isEmpty() ? nullptr : (unsigned char*)(buf.data());
        if((res = libusb_control_transfer(m_p->libusbDeviceHandle, requestType, request, value, index, data, buf.size(), m_timeout)) >= 0) { break; }
        QThread::msleep(RETRY_INTERVAL_MS);
    } while(--retryCount);

    if(res < 0) {
        error_msg(QString("Failed to perform control transfer: %1").arg(libusb_error_name(res)));
    } else if(res != buf.size()) {
        debug_msg("Requested and transferred data size differ");
    } else {}

    return res == buf.size();
}

QByteArray USBDevice::controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, uint16_t length)
{
    QByteArray buf(length, 0);
    int retryCount = RETRY_COUNT, res;

    do {
        if((res = libusb_control_transfer(m_p->libusbDeviceHandle, requestType, request, value, index, (unsigned char*)(buf.data()), length, m_timeout)) >= 0) { break; }
        QThread::msleep(RETRY_INTERVAL_MS);
    } while ((res < 0) && --retryCount);

    if(res < 0) {
        buf.clear();
        error_msg(QString("Failed to perform control transfer: %1").arg(libusb_error_name(res)));
    } else if(res != length) {
        buf.resize(res);
        debug_msg("Requested and transferred data size differ");
    } else {}

    return buf;
}

QByteArray USBDevice::extraInterfaceDescriptor(int interfaceNum, uint8_t type, int length)
{
    QByteArray ret;
    libusb_config_descriptor *cfg;

    check_return_val(!libusb_get_config_descriptor(m_p->libusbDevice, 0, &cfg), "Failed to get configuration descriptor", ret);

    const auto intf = cfg->interface[interfaceNum];

    for(auto i = 0; i < intf.num_altsetting; ++i) {
        const auto altintf = intf.altsetting[i];
        if((altintf.extra_length == length) && (altintf.extra[1] == type)) {
            ret.append((const char*)(altintf.extra), altintf.extra_length);
            break;
        }
    }

    libusb_free_config_descriptor(cfg);

    return ret;
}

QByteArray USBDevice::stringInterfaceDescriptor(uint8_t alt)
{
    libusb_config_descriptor *cfg;

    check_return_val(!libusb_get_config_descriptor(m_p->libusbDevice, 0, &cfg), "Failed to get configuration descriptor", QByteArray());

    const auto BUF_SIZE = 254;
    QByteArray buf(BUF_SIZE, 0);


    const auto intf = *(cfg->interface);
    const auto res = libusb_get_string_descriptor_ascii(m_p->libusbDeviceHandle, intf.altsetting[alt].iInterface, (unsigned char*)buf.data(), BUF_SIZE);

    if(res < 0) {
        error_msg(QString("Failed to get string descriptor: %1").arg(libusb_error_name(res)));
        buf.clear();

    } else {
        buf.resize(res);
    }

    libusb_free_config_descriptor(cfg);

    return buf;
}

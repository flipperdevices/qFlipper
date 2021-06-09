#include "usbdevice.h"

#include <QMutex>

#include <libusb.h>

#include "macros.h"

struct USBDevice::BackendData {
    libusb_device *libusbDevice = nullptr;
    libusb_device_handle *libusbDeviceHandle = nullptr;
};

USBDevice::USBDevice(const USBDeviceInfo &info, QObject *parent):
    QObject(parent),
    m_backendData(new BackendData)
{
    m_backendData->libusbDevice = (libusb_device*)info.uniqueID;
}

USBDevice::~USBDevice()
{
    if(m_isOpen) {
        close();
    }

    delete m_backendData;
}

bool USBDevice::open()
{
    if(!m_isOpen) {
        const auto err = libusb_open(m_backendData->libusbDevice, &(m_backendData->libusbDeviceHandle));
        check_continue(!err, QString("Unable to open device: %1").arg(libusb_error_name(err)));

        m_isOpen = !err;
    }

    return m_isOpen;
}

void USBDevice::close()
{
    if(!m_isOpen) {
        return;
    }

    libusb_close(m_backendData->libusbDeviceHandle);

    m_isOpen = false;
}

bool USBDevice::claimInterface(int interfaceNum)
{
    const auto err = libusb_claim_interface(m_backendData->libusbDeviceHandle, interfaceNum);
    check_continue(!err, QString("Failed to claim interface: %1").arg(libusb_error_name(err)));
    return !err;
}

bool USBDevice::releaseInterface(int interfaceNum)
{
    const auto err = libusb_release_interface(m_backendData->libusbDeviceHandle, interfaceNum);
    check_continue(!err, QString("Failed to release interface: %1").arg(libusb_error_name(err)));
    return !err;
}

bool USBDevice::setInterfaceAltSetting(int interfaceNum, uint8_t alt)
{
    const auto err = libusb_set_interface_alt_setting(m_backendData->libusbDeviceHandle, interfaceNum, alt);
    check_continue(!err, QString("Failed to set alternate setting: %1").arg(libusb_error_name(err)));
    return !err;
}

bool USBDevice::controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, const QByteArray &buf)
{
    const auto res = libusb_control_transfer(m_backendData->libusbDeviceHandle, requestType, request,
                     value, index, buf.isEmpty() ? NULL : (unsigned char*)(buf.data()), buf.size(), m_timeout);

    if(res < 0) {
        error_msg(QString("Failed to perform control transfer: %1").arg(libusb_error_name(res)));
    } else if(res != buf.size()) {
        info_msg("Requested and transferred data size differ");
    } else {}

    return res == buf.size();
}

QByteArray USBDevice::controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, uint16_t length)
{
    QByteArray buf(length, 0);

    const auto res = libusb_control_transfer(m_backendData->libusbDeviceHandle, requestType, request,
                                             value, index, (unsigned char*)(buf.data()), length, m_timeout);
    if(res < 0) {
        buf.clear();
        error_msg(QString("Failed to perform control transfer: %1").arg(libusb_error_name(res)));
    } else if(res != length) {
        buf.resize(res);
        info_msg("Requested and transferred data size differ");
    } else {}

    return buf;
}

QByteArray USBDevice::extraInterfaceDescriptor()
{
    QByteArray ret;
    libusb_config_descriptor *cfg;

    check_return_val(!libusb_get_config_descriptor(m_backendData->libusbDevice, 0, &cfg), "Failed to get configuration descriptor", ret);

    const auto intf = *(cfg->interface); //Using interface 0 for now

    for(auto i = 0; i < intf.num_altsetting; ++i) {
        const auto altintf = intf.altsetting[i];

        // TODO: These are DFU-specific values that should not be here.
        // The code must be refactored to be more general.

        const auto DFU_DESCRIPTOR_LENGTH = 9;
        const auto DFU_DESCRIPTOR_TYPE = 0x21;

        if((altintf.extra_length == DFU_DESCRIPTOR_LENGTH) && (altintf.extra[1] == DFU_DESCRIPTOR_TYPE)) {
            ret.append((const char*)(altintf.extra), altintf.extra_length);
            break;
        }
    }

    libusb_free_config_descriptor(cfg);

    return ret;
}

QByteArray USBDevice::stringInterfaceDescriptor(int interfaceNum)
{
    libusb_config_descriptor *cfg;

    check_return_val(!libusb_get_config_descriptor(m_backendData->libusbDevice, 0, &cfg), "Failed to get configuration descriptor", QByteArray());

    const auto BUF_SIZE = 254;
    QByteArray buf(BUF_SIZE, 0);


    const auto intf = *(cfg->interface);
    const auto res = libusb_get_string_descriptor_ascii(m_backendData->libusbDeviceHandle, intf.altsetting[interfaceNum].iInterface, (unsigned char*)buf.data(), BUF_SIZE);

    if(res < 0) {
        error_msg(QString("Failed to get string descriptor: %1").arg(libusb_error_name(res)));
        buf.clear();

    } else {
        buf.resize(res);
    }

    libusb_free_config_descriptor(cfg);

    return buf;
}

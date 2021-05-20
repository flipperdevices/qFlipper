#include "libusbbackend.h"

#include <QDebug>
#include <exception>
#include <libusb.h>

#include "macros.h"

struct USBBackend::DeviceHandle {
    libusb_device *libusbDevice = nullptr;
    libusb_device_handle *libusbDeviceHandle = nullptr;
};

static constexpr const char* dbgLabel = "libusb backend:";

USBBackend::USBBackend()
{
//    throw std::runtime_error("Fakie nollie");

    if(libusb_init(nullptr)) {
        throw std::runtime_error("Failed to initialise libusb");
    }
}

USBBackend::~USBBackend()
{
    libusb_exit(nullptr);
}

USBBackend::DeviceList USBBackend::listDevices()
{
    USBBackend::DeviceList ret;

    if(libusb_init(nullptr)) {
        throw std::runtime_error("Failed to initialise libusb");
    }

    libusb_device **list;
    const auto numDevs = libusb_get_device_list(nullptr, &list);

    for(auto i = 0; i < numDevs; ++i) {
        struct libusb_device *dev = list[i];
        struct libusb_device_descriptor desc;

        if(const auto err = libusb_get_device_descriptor(dev, &desc)) {
            error_msg(QString("Failed to get device descriptor: ") + libusb_error_name(err));
            continue;
        }

        //TODO: Determine how much of these fields are actually needed
        USBDeviceLocation loc;

        loc.vendorID = desc.idVendor;
        loc.productID = desc.idProduct;

        loc.busNumber = libusb_get_bus_number(dev);
        loc.address = libusb_get_device_address(dev);

        struct libusb_device_handle *handle;
        if(const auto err = libusb_open(dev, &handle)) {
            error_msg(QString("Failed to open device: ") + libusb_error_name(err));
            continue;
        }

        unsigned char buf[0xff];

        if(libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, buf, sizeof(buf)) < 0) {
            error_msg("Failed to get manufacturer string descriptor");
        } else {
            loc.manufacturer = QString::fromLocal8Bit((const char*)buf);
        }

        if(libusb_get_string_descriptor_ascii(handle, desc.iProduct, buf, sizeof(buf)) < 0) {
            error_msg("Failed to get product string descriptor");
        } else {
            loc.productDescription = QString::fromLocal8Bit((const char*)buf);
        }

        if(libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, buf, sizeof(buf)) < 0) {
            error_msg("Failed to get device serial number");
        } else {
            loc.serialNumber = QString::fromLocal8Bit((const char*)buf);
        }

        ret.append(loc);

        libusb_close(handle);
    }

    libusb_free_device_list(list, 1);
    libusb_exit(nullptr);

    return ret;
}

bool USBBackend::findDevice(DeviceHandle **handle, const USBDeviceLocation &loc)
{
    bool res = false;

    libusb_device **list;
    const auto numDevs = libusb_get_device_list(nullptr, &list);

    for(auto i = 0; i < numDevs; ++i) {
        libusb_device *dev = list[i];
        libusb_device_descriptor desc;

        if(const auto err = libusb_get_device_descriptor(dev, &desc)) {
            qCritical() << dbgLabel << "Failed to get device descriptor:" << libusb_error_name(err);
            continue;
        }

        const auto vendorOK = (loc.vendorID == desc.idVendor);
        const auto productOK = (loc.productID == desc.idProduct);
        const auto busOK = (loc.busNumber == libusb_get_bus_number(dev));
        const auto addrOK = (loc.address == libusb_get_device_address(dev));

        res = vendorOK && productOK && busOK && addrOK;

        if(res) {
            *handle = new DeviceHandle;
            (*handle)->libusbDevice = libusb_ref_device(dev);
            break;
        }
    }

    libusb_free_device_list(list, 1);
    return res;
}

void USBBackend::unrefDevice(DeviceHandle *handle)
{
    libusb_unref_device(handle->libusbDevice);
}

QByteArray USBBackend::getExtraInterfaceDescriptor(DeviceHandle *handle)
{
    QByteArray ret;
    libusb_config_descriptor *cfg;

    if(const auto err = libusb_get_config_descriptor(handle->libusbDevice, 0, &cfg)) {
        qCritical() << dbgLabel << "Failed to get configuration descriptor";
        return ret;
    }

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

QByteArray USBBackend::getStringInterfaceDescriptor(DeviceHandle *handle, int interfaceNum)
{
    libusb_config_descriptor *cfg;

    if(const auto err = libusb_get_config_descriptor(handle->libusbDevice, 0, &cfg)) {
        qCritical() << dbgLabel << "Failed to get configuration descriptor";
        return QByteArray();
    }

    const auto BUF_SIZE = 254;
    QByteArray buf(BUF_SIZE, 0);


    const auto intf = *(cfg->interface);
    const auto res = libusb_get_string_descriptor_ascii(handle->libusbDeviceHandle, intf.altsetting[interfaceNum].iInterface, (unsigned char*)buf.data(), BUF_SIZE);

    if(res < 0) {
        qCritical() << dbgLabel << "Failed to get string descriptor:" << libusb_error_name(res);
        buf.clear();
    } else {
        buf.resize(res);
    }

    libusb_free_config_descriptor(cfg);

    return buf;
}

bool USBBackend::openDevice(DeviceHandle *handle)
{
    const auto err = libusb_open(handle->libusbDevice, &(handle->libusbDeviceHandle));

    if(err) {
        qCritical() << dbgLabel << "Unable to open device:" << libusb_error_name(err);
    }

    return !err;
}

void USBBackend::closeDevice(DeviceHandle *handle)
{
    // TODO: check if the device is open?
    libusb_close(handle->libusbDeviceHandle);
}

bool USBBackend::claimInterface(DeviceHandle *handle, int interfaceNum)
{
    const auto err = !libusb_claim_interface(handle->libusbDeviceHandle, interfaceNum);
    check_continue(err, "Failed to claim interface");
    return err;
}

bool USBBackend::releaseInterface(DeviceHandle *handle, int interfaceNum)
{
    const auto err = !libusb_release_interface(handle->libusbDeviceHandle, interfaceNum);
    check_continue(err, "Failed to release interface");
    return err;
}

bool USBBackend::setInterfaceAltSetting(DeviceHandle *handle, int interfaceNum, uint8_t alt)
{
    const auto err = !libusb_set_interface_alt_setting(handle->libusbDeviceHandle, interfaceNum, alt);
    check_continue(err, "Failed to set alternate setting");
    return err;
}

bool USBBackend::controlTransfer(DeviceHandle *handle, uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, const QByteArray &buf)
{
    const auto res = libusb_control_transfer(handle->libusbDeviceHandle, requestType, request,
                     value, index, buf.isEmpty() ? NULL : (unsigned char*)(buf.data()), buf.size(), m_timeout);

    if(res < 0) {
        qCritical() << dbgLabel << "(OUT): Failed to perform control transfer" << libusb_error_name(res);
    } else if(res != buf.size()) {
        qInfo() << dbgLabel << "(OUT): Requested and transferred data size differ";
    } else {}

    return res == buf.size();
}

QByteArray USBBackend::controlTransfer(DeviceHandle *handle, uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, uint16_t length)
{
    QByteArray buf(length, 0);

    const auto res = libusb_control_transfer(handle->libusbDeviceHandle, requestType, request,
                                             value, index, (unsigned char*)(buf.data()), length, m_timeout);

    if(res < 0) {
        buf.clear();
        qCritical() << dbgLabel << "(IN): Failed to perform control transfer:" << libusb_error_name(res);

    } else if(res != length) {
        buf.resize(res);
        qInfo() << dbgLabel << "(IN): Requested and transferred data size differ";
    } else {}

    return buf;
}

unsigned int USBBackend::m_timeout = 10000;

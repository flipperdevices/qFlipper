#include "libusbusbdevicebackend.h"

#include <QDebug>

#include <libusb.h>

struct LibusbUSBDeviceBackend::BackendData {
    libusb_context *context;
    libusb_device *device;
    libusb_device_handle *handle;

    unsigned int timeout = 1000;
};

static constexpr const char* dbgLabel = "libusb backend:";

LibusbUSBDeviceBackend::LibusbUSBDeviceBackend(QObject *parent):
    AbstractUSBDeviceBackend(parent),
    m_pdata(new BackendData)
{}

LibusbUSBDeviceBackend::~LibusbUSBDeviceBackend()
{
    delete m_pdata;
}

bool LibusbUSBDeviceBackend::init()
{
    const auto err = libusb_init(&(m_pdata->context));

    if(err) {
        m_pdata->context = nullptr;
        qCritical() << dbgLabel << "Failed to initialise libusb:" << libusb_error_name(err);
    }

    return !err;
}

void LibusbUSBDeviceBackend::exit()
{
    // TODO: Check for open devices?
    if(m_pdata->context) {
        libusb_exit(m_pdata->context);
    }
}

bool LibusbUSBDeviceBackend::findDevice(const USBDeviceLocation &loc)
{
    bool res = false;

    libusb_device **list;
    const auto numDevs = libusb_get_device_list(m_pdata->context, &list);

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
            m_pdata->device = libusb_ref_device(dev);
            break;
        }
    }

    libusb_free_device_list(list, 1);
    return res;
}

QByteArray LibusbUSBDeviceBackend::getExtraInterfaceDescriptor()
{
    QByteArray ret;
    libusb_config_descriptor *cfg;

    if(const auto err = libusb_get_config_descriptor(m_pdata->device, 0, &cfg)) {
        qCritical() << dbgLabel << "Failed to get configuration descriptor";
        return ret;
    }

    const auto intf = *(cfg->interface);

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

bool LibusbUSBDeviceBackend::openDevice()
{
    const auto err = libusb_open(m_pdata->device, &(m_pdata->handle));

    if(err) {
        qCritical() << dbgLabel << "Unable to open device:" << libusb_error_name(err);
    }

    return !err;
}

void LibusbUSBDeviceBackend::closeDevice()
{
    // TODO: check if the device is open?
    libusb_close(m_pdata->handle);
}

bool LibusbUSBDeviceBackend::claimInterface(int interfaceNum)
{
    const auto err = libusb_claim_interface(m_pdata->handle, interfaceNum);

    if(err) {
        qCritical() << dbgLabel << "Failed to claim interface";
    }

    return !err;
}

bool LibusbUSBDeviceBackend::releaseInterface(int interfaceNum)
{
    const auto err = libusb_release_interface(m_pdata->handle, interfaceNum);

    if(err) {
        qCritical() << dbgLabel << "Failed to release interface";
    }

    return !err;
}

bool LibusbUSBDeviceBackend::controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, const QByteArray &buf)
{
    const auto res = libusb_control_transfer(m_pdata->handle, requestType, request,
                     value, index, buf.isEmpty() ? NULL : (unsigned char*)(buf.data()), buf.size(), m_pdata->timeout);

    if(res < 0) {
        qCritical() << dbgLabel << "(OUT): Failed to perform control transfer" << libusb_error_name(res);
    } else if(res != buf.size()) {
        qInfo() << dbgLabel << "(OUT): Requested and transferred data size differ";
    } else {}

    return res == buf.size();
}

QByteArray LibusbUSBDeviceBackend::controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, uint16_t length)
{
    QByteArray buf(length, 0);

    const auto res = libusb_control_transfer(m_pdata->handle, requestType, request,
                                             value, index, (unsigned char*)(buf.data()), length, m_pdata->timeout);

    if(res < 0) {
        buf.clear();
        qCritical() << dbgLabel << "(IN): Failed to perform control transfer:" << libusb_error_name(res);

    } else if(res != length) {
        buf.resize(res);
        qInfo() << dbgLabel << "(IN): Requested and transferred data size differ";
    } else {}

    return buf;
}

#include "libusbusbdevicebackend.h"

#include <QDebug>

#include <libusb.h>

struct LibusbUSBDeviceBackend::BackendData {
    libusb_context *context;
    libusb_device *device;
    libusb_device_handle *handle;

    libusb_device_descriptor deviceDescriptor;
    libusb_config_descriptor configDescriptor;

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
    if(m_pdata->context) {
        libusb_exit(m_pdata->context);
    }
}

bool LibusbUSBDeviceBackend::findDevice(const USBDeviceLocation &loc)
{
    libusb_device **list;
    const auto numDevs = libusb_get_device_list(m_pdata->context, &list);

    for(auto i = 0; i < numDevs; ++i) {
        libusb_device *dev = list[i];
        libusb_device_descriptor desc;

        if(const auto err = libusb_get_device_descriptor(dev, &desc)) {
            qCritical() << "libusb backend: Failed to get device descriptor:" << libusb_error_name(err);
            continue;
        }

        const auto vendorOK = (loc.vendorID == desc.idVendor);
        const auto productOK = (loc.productID == desc.idProduct);
        const auto busOK = (loc.busNumber == libusb_get_bus_number(dev));
        const auto addrOK = (loc.address == libusb_get_device_address(dev));

        if(vendorOK && productOK && busOK && addrOK) {
            m_pdata->device = libusb_ref_device(dev);
            m_pdata->deviceDescriptor = desc;

            return true;
        }
    }

    libusb_free_device_list(list, 1);
    return false;
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
        qCritical() << dbgLabel << "Failed to perform control transfer [OUT]:" << libusb_error_name(res);
    } else if(res != buf.size()) {
        qCritical() << dbgLabel << "Failed to transfer all data [OUT]";
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
        qCritical() << dbgLabel << "Failed to perform control transfer [IN]:" << libusb_error_name(res);

    } else if(res != length) {
        buf.resize(res);
        qCritical() << dbgLabel << "Failed to transfer all data [IN]";
    } else {}

    return buf;
}

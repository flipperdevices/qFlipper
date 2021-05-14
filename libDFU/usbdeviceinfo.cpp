#include "usbdeviceinfo.h"

#include <QDebug>

// TODO: Abstract libusb via backend
#include <libusb.h>

QList<USBDeviceInfo> USBDeviceInfo::availableDevices(const USBDeviceInfo::MatchList &devsToMatch)
{
    QList<USBDeviceInfo> devices;

    if(const auto err = libusb_init(NULL)) {
        qCritical() << "DFU: Unable to initialise libusb:" << err;
        return devices;
    }

    libusb_device **list;
    const auto numDevs = libusb_get_device_list(NULL, &list);

    for(auto i = 0; i < numDevs; ++i) {
        struct libusb_device *dev = list[i];
        struct libusb_device_descriptor desc;

        if(const auto err = libusb_get_device_descriptor(dev, &desc)) {
            qCritical() << "DFU: Can't get device descriptor:" << err;
            continue;
        }

        const auto data = tryMatchDevice(dev, desc, devsToMatch);

        if(!data.isValid) {
            if(i == numDevs - 1) break;
            else continue;
        }

        devices.append(USBDeviceInfo(data));

    }

    libusb_free_device_list(list, 0);
    libusb_exit(NULL);

    return devices;
}

USBDeviceInfo::USBDeviceInfo(const USBDeviceLocation &data):
    m_data(data)
{}

bool USBDeviceInfo::isValid() const
{
    return m_data.isValid;
}

uint8_t USBDeviceInfo::busNumber() const
{
    return m_data.busNumber;
}

uint8_t USBDeviceInfo::address() const
{
    return m_data.address;
}

uint16_t USBDeviceInfo::vendorIdentifier() const
{
    return m_data.vendorID;
}

uint16_t USBDeviceInfo::productIdentifier() const
{
    return m_data.productID;
}

const QString &USBDeviceInfo::manufacturer() const
{
    return m_data.manufacturer;
}

const QString &USBDeviceInfo::description() const
{
    return m_data.productDescription;
}

const QString &USBDeviceInfo::serialNumber() const
{
    return m_data.serialNumber;
}

const USBDeviceLocation &USBDeviceInfo::data() const
{
    return m_data;
}

USBDeviceLocation USBDeviceInfo::tryMatchDevice(libusb_device *dev, libusb_device_descriptor &desc, const MatchList &matches)
{
    USBDeviceLocation ret;
    int matchIdx = -1;

    for(auto i = 0; i < matches.size(); ++i) {
        const auto &match = matches[i];
        if((desc.idVendor == match.vendorIdentifier) && (desc.idProduct == match.productIdentifier)) {
            matchIdx = i;
            break;
        }
    }

    if(matchIdx < 0) {
        // vid:pid didn't match, giving up
        return ret;
    }

    const auto &match = matches[matchIdx];

    ret.vendorID = match.vendorIdentifier;
    ret.productID = match.productIdentifier;

    ret.busNumber = libusb_get_bus_number(dev);
    ret.address = libusb_get_device_address(dev);

    if(match.manufacturer.isEmpty() && match.productDescription.isEmpty()) {
        ret.isValid = true;
        return ret;
    }

    // we need to check these as well then
    struct libusb_device_handle *handle;
    if(const auto err = libusb_open(dev, &handle)) {
        qCritical() << "DFU: VID:PID matches, but couldn't open device:" << err;
        return ret;
    }

    unsigned char buf[0xff];

    auto closeAndReturn = [handle, &ret]() -> USBDeviceLocation& {
        libusb_close(handle);
        return ret;
    };

    if(libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, buf, sizeof(buf)) < 0) {
        qCritical() << "DFU: VID:PID matches, but couldn't get manufacturer string";
        return closeAndReturn();
    }

    if(!match.manufacturer.isEmpty()) {
        ret.manufacturer = QString::fromLatin1((const char*)buf);

        if(libusb_get_string_descriptor_ascii(handle, desc.iProduct, buf, sizeof(buf)) < 0) {
            qCritical() << "DFU: VID:PID matches, but couldn't get product string";
            return closeAndReturn();
        }
    }

    if(!match.productDescription.isEmpty()) {
        ret.productDescription = QString::fromLatin1((const char*)buf);

        if(libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, buf, sizeof(buf)) < 0) {
            qInfo() << "DFU: Can't get device serial number";
            return closeAndReturn();
        }
    }

    ret.serialNumber = QString::fromLatin1((const char*)buf);

    ret.isValid = (ret.manufacturer == match.manufacturer) && (ret.productDescription == match.productDescription);

    if(!ret.isValid) {
        qInfo() << "DFU: VID:PID matches, but not the manufacturer or product";
    }

    return closeAndReturn();
}

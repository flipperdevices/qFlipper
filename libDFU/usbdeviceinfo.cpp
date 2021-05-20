#include "usbdeviceinfo.h"

#include <QDebug>

#include "usbbackend.h"

QList<USBDeviceInfo> USBDeviceInfo::availableDevices(const USBDeviceInfo::MatchList &devsToMatch)
{
    QList<USBDeviceInfo> infos;

    const auto list = USBBackend::listDevices();

    for(const auto &item : list) {
        for(const auto &match : devsToMatch) {
            if(item.vendorID != match.vendorIdentifier) continue;
            if(item.productID != match.productIdentifier) continue;

            if((!match.manufacturer.isEmpty()) && (item.manufacturer != match.manufacturer)) continue;
            if((!match.productDescription.isEmpty()) && (item.productDescription != match.productDescription)) continue;
            if((!match.serialNumber.isEmpty()) && (item.serialNumber != match.serialNumber)) continue;

            infos.append(item);
            break;
        }
    }

    return infos;
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

#include "usbdeviceinfo.h"

USBDeviceInfo::USBDeviceInfo(uint16_t vendorID, uint16_t productID):
    m_vendorID(vendorID),
    m_productID(productID)
{}

bool USBDeviceInfo::isComplete() const
{
    return m_vendorID != 0 && m_productID != 0 &&
#ifdef USB_BACKEND_LIBUSB
          !m_manufacturer.isEmpty() && !m_productDescription.isEmpty() &&
#endif
          !m_serialNumber.isEmpty() && m_backendData.isValid();
}

USBDeviceInfo USBDeviceInfo::withBackendData(const QVariant &backendData) const
{
    auto ret = *this;
    ret.m_backendData = backendData;
    return ret;
}

USBDeviceInfo USBDeviceInfo::withSerialNumber(const QString &serialNumber) const
{
    auto ret = *this;
    ret.m_serialNumber = serialNumber;
    return ret;
}

USBDeviceInfo USBDeviceInfo::withManufacturer(const QString &manufacturer) const
{
    auto ret = *this;
    ret.m_manufacturer = manufacturer;
    return ret;
}

USBDeviceInfo USBDeviceInfo::withProductDescription(const QString &productDescription) const
{
    auto ret = *this;
    ret.m_productDescription = productDescription;
    return ret;
}

uint16_t USBDeviceInfo::vendorID() const
{
    return m_vendorID;
}

uint16_t USBDeviceInfo::productID() const
{
    return m_productID;
}

const QString &USBDeviceInfo::manufacturer() const
{
    return m_manufacturer;
}

const QString &USBDeviceInfo::productDescription() const
{
    return m_productDescription;
}

const QString &USBDeviceInfo::serialNumber() const
{
    return m_serialNumber;
}

const QVariant &USBDeviceInfo::backendData() const
{
    return m_backendData;
}

void USBDeviceInfo::setManufacturer(const QString &manufacturer)
{
    m_manufacturer = manufacturer;
}

void USBDeviceInfo::setProductDescription(const QString &productDescription)
{
    m_productDescription = productDescription;
}

void USBDeviceInfo::setSerialNumber(const QString &serialNumber)
{
    m_serialNumber = serialNumber;
}

bool USBDeviceInfo::operator ==(const USBDeviceInfo &other) const
{
    return m_backendData == other.m_backendData;
}

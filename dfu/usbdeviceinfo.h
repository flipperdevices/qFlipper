#pragma once

#include <QString>
#include <QVariant>
#include <QMetaType>

class USBDeviceInfo {
public:
    USBDeviceInfo() = default;
    USBDeviceInfo(uint16_t vendorID, uint16_t productID);

    bool isComplete() const;

    USBDeviceInfo withBackendData(const QVariant &backendData) const;
    USBDeviceInfo withSerialNumber(const QString &serialNumber) const;
    USBDeviceInfo withManufacturer(const QString &manufacturer) const;
    USBDeviceInfo withProductDescription(const QString &productDescription) const;

    uint16_t vendorID() const;
    uint16_t productID() const;

    const QString &manufacturer() const;
    const QString &productDescription() const;
    const QString &serialNumber() const;

    const QVariant &backendData() const;

    void setManufacturer(const QString &manufacturer);
    void setProductDescription(const QString &productDescription);
    void setSerialNumber(const QString &serialNumber);

    bool operator ==(const USBDeviceInfo &other) const;

private:
    uint16_t m_vendorID;
    uint16_t m_productID;

    QString m_manufacturer;
    QString m_productDescription;
    QString m_serialNumber;

    QVariant m_backendData;
};

Q_DECLARE_METATYPE(USBDeviceInfo)

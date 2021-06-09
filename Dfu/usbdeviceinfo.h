#ifndef USBDEVICEINFO_H
#define USBDEVICEINFO_H

#include <QString>
#include <QMetaType>

struct USBDeviceInfo {
    uint16_t vendorID;
    uint16_t productID;

    QString manufacturer;
    QString productDescription;
    QString serialNumber;

    // Very questionable - disrupts abstraction
    void *uniqueID;
};

Q_DECLARE_METATYPE(USBDeviceInfo)

#endif // USBDEVICEINFO_H

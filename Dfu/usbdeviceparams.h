#ifndef USBDEVICEINFODATA_H
#define USBDEVICEINFODATA_H

#include <QString>
#include <QMetaType>

struct USBDeviceParams {
    uint16_t vendorID;
    uint16_t productID;

    QString manufacturer;
    QString productDescription;
    QString serialNumber;

    // Very questionable - disrupts abstraction
    void *uniqueID;
};

Q_DECLARE_METATYPE(USBDeviceParams)

#endif // USBDEVICEINFODATA_H

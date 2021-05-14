#ifndef USBDEVICEINFODATA_H
#define USBDEVICEINFODATA_H

#include <QString>

// Made to avoid possible circular dependencies
// TODO: incorporate back to USBDeviceInfo if it turns out not to be the case
struct USBDeviceLocation {
    bool isValid = false;

    uint8_t busNumber;
    uint8_t address;

    uint16_t vendorID;
    uint16_t productID;

    QString manufacturer;
    QString productDescription;
    QString serialNumber;
};

#endif // USBDEVICEINFODATA_H

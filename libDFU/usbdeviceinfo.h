#ifndef USBDEVICEINFO_H
#define USBDEVICEINFO_H

#include <QList>
#include <QVector>

// TODO: Refactor to use USB backend abstraction

struct libusb_device;
struct libusb_device_descriptor;

#include "usbdevicelocation.h"

class USBDeviceInfo
{
public:
    struct MatchType {
        uint16_t vendorIdentifier;
        uint16_t productIdentifier;

        QString manufacturer;
        QString productDescription;
        QString serialNumber;
    };

    using MatchList = QVector<MatchType>;

    static QList<USBDeviceInfo> availableDevices(const MatchList &devsToMatch);

    USBDeviceInfo() = default;
    USBDeviceInfo(const USBDeviceLocation &data);

    bool isValid() const;

    uint8_t busNumber() const;
    uint8_t address() const;

    uint16_t vendorIdentifier() const;
    uint16_t productIdentifier() const;

    const QString &manufacturer() const;
    const QString &description() const;
    const QString &serialNumber() const;

    const USBDeviceLocation &data() const;

private:
    static USBDeviceLocation tryMatchDevice(struct libusb_device *dev, struct libusb_device_descriptor &desc, const MatchList &matches);
    USBDeviceLocation m_data;
};

#endif // USBDEVICEINFO_H

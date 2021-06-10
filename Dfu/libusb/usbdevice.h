#ifndef LIBUSBDEVICE_H
#define LIBUSBDEVICE_H

#include <QObject>

#include "usbdeviceinfo.h"

class QMutex;
// TODO: Multithreading improvements

class USBDevice : public QObject
{
    Q_OBJECT

    struct USBDevicePrivate;

public:
    enum EndpointDirection {
        ENDPOINT_IN = 0x80,
        ENDPOINT_OUT = 0x00,
    };

    enum RequestType {
        REQUEST_TYPE_STANDARD = 0x00,
        REQUEST_TYPE_CLASS = (0x01 << 5),
        REQUEST_TYPE_VENDOR = (0x02 << 5),
        REQUEST_TYPE_RESERVED = (0x03 << 5)
    };

    enum RequestRecipient {
        RECIPIENT_DEVICE = 0x00,
        RECIPIENT_INTERFACE = 0x01,
        RECIPIENT_ENDPOINT = 0x02,
        RECIPIENT_OTHER = 0x03
    };

    USBDevice(const USBDeviceInfo &info, QObject *parent = nullptr);
    virtual ~USBDevice();

    bool open();
    void close();

    bool claimInterface(int interfaceNum);
    bool releaseInterface(int interfaceNum);
    bool setInterfaceAltSetting(int interfaceNum, uint8_t alt);

    bool controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, const QByteArray &data);
    QByteArray controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, uint16_t length);

    QByteArray extraInterfaceDescriptor(int interfaceNum, uint8_t type, int length);
    QByteArray stringInterfaceDescriptor(int interfaceNum);

private:
    USBDevicePrivate *m_p = nullptr;
    unsigned long m_timeout = 1000;
};

#endif // LIBUSBDEVICE_H

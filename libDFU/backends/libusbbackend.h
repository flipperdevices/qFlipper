#ifndef LIBUSBBACKEND_H
#define LIBUSBBACKEND_H

#include <QByteArray>

#include "usbdevicelocation.h"

class USBBackend
{
public:
    struct DeviceHandle;

    using DeviceList = QList<USBDeviceLocation>;

    USBBackend();
    ~USBBackend();

    static DeviceList listDevices();

    bool findDevice(DeviceHandle **handle, const USBDeviceLocation &loc);
    void unrefDevice(DeviceHandle *handle);

    QByteArray getExtraInterfaceDescriptor(DeviceHandle *handle);
    QByteArray getStringInterfaceDescriptor(DeviceHandle *handle, int interfaceNum);

    bool openDevice(DeviceHandle *handle);
    void closeDevice(DeviceHandle *handle);

    bool claimInterface(DeviceHandle *handle, int interfaceNum);
    bool releaseInterface(DeviceHandle *handle, int interfaceNum);

    bool setInterfaceAltSetting(DeviceHandle *handle, int interfaceNum, uint8_t alt);

    bool controlTransfer(DeviceHandle *handle, uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, const QByteArray &buf);
    QByteArray controlTransfer(DeviceHandle *handle, uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, uint16_t length);

private:
    static unsigned int m_timeout;
};

#endif // LIBUSBBACKEND_H

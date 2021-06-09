#ifndef LIBUSBBACKEND_H
#define LIBUSBBACKEND_H

#include <QObject>
#include <QByteArray>

#include "usbdeviceparams.h"

class USBDeviceDetector;
class USBBackend : public QObject
{
    Q_OBJECT

public:
    struct DeviceHandle;

    USBBackend(QObject *parent = nullptr);
    ~USBBackend();

    static USBBackend *instance();
    USBDeviceDetector *detector();

    void initDevice(DeviceHandle **handle, const USBDeviceParams &params);
    // TODO: needed?
    void unrefDevice(DeviceHandle *handle);

    bool openDevice(DeviceHandle *handle);
    void closeDevice(DeviceHandle *handle);

    bool claimInterface(DeviceHandle *handle, int interfaceNum);
    bool releaseInterface(DeviceHandle *handle, int interfaceNum);

    bool setInterfaceAltSetting(DeviceHandle *handle, int interfaceNum, uint8_t alt);

    bool controlTransfer(DeviceHandle *handle, uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, const QByteArray &buf);
    QByteArray controlTransfer(DeviceHandle *handle, uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, uint16_t length);

    // TODO: these methods are ugly, do something about them
    QByteArray getExtraInterfaceDescriptor(DeviceHandle *handle);
    QByteArray getStringInterfaceDescriptor(DeviceHandle *handle, int interfaceNum);

    // TODO: move this method in USBDevice? (add getStringDescriptor function instead)
    static USBDeviceParams getExtraDeviceInfo(const USBDeviceParams &params);

private:
    USBDeviceDetector *m_detector;
    unsigned int m_timeout = 1000;
};

#endif // LIBUSBBACKEND_H

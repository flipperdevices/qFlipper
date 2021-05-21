#ifndef LIBUSBBACKEND_H
#define LIBUSBBACKEND_H

#include <QObject>
#include <QByteArray>

#include "usbdeviceparams.h"

class USBBackend : public QObject
{
    Q_OBJECT

public:
    struct DeviceHandle;

    using DeviceList = QList<USBDeviceParams>;

    USBBackend(QObject *parent = nullptr);
    ~USBBackend();

    static bool getExtraDeviceInfo(USBDeviceParams &params);

    bool findDevice(DeviceHandle **handle, const USBDeviceParams &params);
    void unrefDevice(DeviceHandle *handle);

    bool registerHotplugEvent(const DeviceList &paramsList);

    QByteArray getExtraInterfaceDescriptor(DeviceHandle *handle);
    QByteArray getStringInterfaceDescriptor(DeviceHandle *handle, int interfaceNum);

    bool openDevice(DeviceHandle *handle);
    void closeDevice(DeviceHandle *handle);

    bool claimInterface(DeviceHandle *handle, int interfaceNum);
    bool releaseInterface(DeviceHandle *handle, int interfaceNum);

    bool setInterfaceAltSetting(DeviceHandle *handle, int interfaceNum, uint8_t alt);

    bool controlTransfer(DeviceHandle *handle, uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, const QByteArray &buf);
    QByteArray controlTransfer(DeviceHandle *handle, uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, uint16_t length);

signals:
    void devicePluggedIn(USBDeviceParams);
    void deviceUnplugged(USBDeviceParams);

private:
    void timerEvent(QTimerEvent *e) override;

    static unsigned int m_timeout;
};

#endif // LIBUSBBACKEND_H

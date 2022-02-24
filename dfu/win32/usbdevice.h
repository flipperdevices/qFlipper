#pragma once

#include <QObject>

#include "usbdeviceinfo.h"

class USBDevice : public QObject
{
    Q_OBJECT

    struct USBDevicePrivate;

public:
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
    QByteArray stringInterfaceDescriptor(uint8_t alt);

private:
    bool m_isOpen = false;
    unsigned long m_timeout = 1000;

    USBDevicePrivate *m_p;
};

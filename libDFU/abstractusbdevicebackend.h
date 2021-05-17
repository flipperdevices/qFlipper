#ifndef ABSTRACTUSBBACKEND_H
#define ABSTRACTUSBBACKEND_H

#include <QObject>
#include <QByteArray>

#include "usbdevicelocation.h"

class AbstractUSBDeviceBackend : public QObject
{
    Q_OBJECT

public:
    AbstractUSBDeviceBackend(QObject *parent = nullptr);

    virtual bool init() = 0;
    virtual void exit() = 0;

    virtual bool findDevice(const USBDeviceLocation &loc) = 0;

    virtual QByteArray getExtraInterfaceDescriptor() = 0;

    virtual bool openDevice() = 0;
    virtual void closeDevice() = 0;

    virtual bool claimInterface(int interfaceNum) = 0;
    virtual bool releaseInterface(int interfaceNum) = 0;

    virtual bool controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, const QByteArray &data) = 0;
    virtual QByteArray controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, uint16_t length) = 0;
};

#endif // ABSTRACTUSBBACKEND_H

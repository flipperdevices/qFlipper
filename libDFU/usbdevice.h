#ifndef USBDEVICE_H
#define USBDEVICE_H

#include <QObject>

#include "usbdeviceinfo.h"

/*
 * Assumptions made:
 * 1) Single device descriptor (as per DFU protocol specification)
 * 2) Single configuration descriptor (see above)
 * 3) Single interface descriptor (see above)
 * 4) Multiple alternate configurations
 * 5) For now, only control transfers are supported
 *
 * Therefore, this class only provides access to alternate configurations
 * of the first interface of the first configuration.
*/

class AbstractUSBDeviceBackend;

class USBDevice : public QObject
{
    Q_OBJECT

public:
    USBDevice(const USBDeviceInfo &info, QObject *parent = nullptr);
    virtual ~USBDevice();

    bool open();
    void close();

    void reenumerate();

    bool claimInterface(int interfaceNum);
    bool releaseInterface(int interfaceNum);

    bool controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, const QByteArray &data);
    QByteArray controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, uint16_t length);

signals:

private:
    USBDeviceLocation m_location;
    AbstractUSBDeviceBackend *m_backend;
};

#endif // USBDEVICE_H

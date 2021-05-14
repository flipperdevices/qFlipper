#ifndef LIBUSBUSBBACKEND_H
#define LIBUSBUSBBACKEND_H

#include "abstractusbdevicebackend.h"

class LibusbUSBDeviceBackend : public AbstractUSBDeviceBackend
{
    Q_OBJECT

public:
    LibusbUSBDeviceBackend(QObject *parent = nullptr);
    ~LibusbUSBDeviceBackend();

    bool init() override;
    void exit() override;

    bool findDevice(const USBDeviceLocation &loc) override;

    bool openDevice() override;
    void closeDevice() override;

    bool claimInterface(int interfaceNum) override;
    bool releaseInterface(int interfaceNum) override;

    bool controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, const QByteArray &buf) override;
    QByteArray controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, uint16_t length) override;

private:
    struct BackendData;

    BackendData *m_pdata;
};

#endif // LIBUSBUSBBACKEND_H

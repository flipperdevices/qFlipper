#ifndef DFUDEVICE_H
#define DFUDEVICE_H

#include <QIODevice>

#include "usbdevice.h"

class DFUDevice : public USBDevice
{
    Q_OBJECT

public:
    struct Status {
        uint8_t status = 0xff;
        uint32_t timeout;
        uint8_t state;
        uint8_t istring;

        bool isValid() const { return status != 0xff; }
    };

    DFUDevice(const USBDeviceInfo &info, QObject *parent = nullptr);

    bool clearStatus();
    Status getStatus();

    bool download(QIODevice &file, int alt = 0);
    bool upload(QIODevice &file, int alt = 0);
};

#endif // DFUDEVICE_H

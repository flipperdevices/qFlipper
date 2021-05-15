#ifndef DFUDEVICE_H
#define DFUDEVICE_H

#include <QIODevice>
#include <QMetaEnum>

#include "usbdevice.h"

class DFUDevice : public USBDevice
{
    Q_OBJECT

public:
    struct Status {
        enum Error {
            OK = 0,
            ERR_TARGET,
            ERR_FILE,
            ERR_WRITE,
            ERR_ERASE,
            ERR_CHECK_ERASED,
            ERR_PROG,
            ERR_VERIFY,
            ERR_ADDRESS,
            ERR_NOTDONE,
            ERR_FIRMWARE,
            ERR_VENDOR,
            ERR_USBR,
            ERR_POR,
            ERR_UNKNOWN,
            ERR_STALLEDPKT,

            ERR_UNDEFINED = 0xff
        };

        enum State {
            APP_IDLE = 0,
            APP_DETACH,
            DFU_IDLE,
            DFU_DNLOAD_SYNC,
            DFU_DNBUSY,
            DFU_DNLOAD_IDLE,
            DFU_MANIFEST_SYNC,
            DFU_MANIFEST,
            DFU_MANIFEST_WAIT_RESET,
            DFU_UPLOAD_IDLE,
            DFU_ERROR,

            STATE_UNDEFINED = 0xff
        };

        Error error = ERR_UNDEFINED;
        State state = STATE_UNDEFINED;

        uint32_t timeout;
        uint8_t istring;
    };

    enum Request {
        DFU_DETACH = 0,
        DFU_DNLOAD,
        DFU_UPLOAD,
        DFU_GETSTATUS,
        DFU_CLRSTATUS,
        DFU_GETSTATE,
        DFU_ABORT
    };

    DFUDevice(const USBDeviceInfo &info, QObject *parent = nullptr);

    bool beginTransaction(int alt = 0);
    bool endTransaction();

    bool abort();

    bool clearStatus();
    Status getStatus();

    bool download(QIODevice &file, uint32_t addr);
    bool upload(QIODevice &file, uint32_t addr, size_t len);

private:
    bool setAddressPointer(uint32_t addr);
};


#endif // DFUDEVICE_H

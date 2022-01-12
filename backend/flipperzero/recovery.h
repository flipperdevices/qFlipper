#pragma once

#include <QObject>

#include "failable.h"
#include "usbdeviceinfo.h"

class QIODevice;

namespace Flipper {
namespace Zero {

class DeviceState;

class Recovery : public QObject, public Failable
{
    Q_OBJECT

public:
    enum class BootMode {
        Normal,
        DFUOnly
    };

    enum class WirelessStatus {
        WSRunning,
        FUSRunning,
        ErrorOccured,
        UnhandledState,
        Invalid
    };

    Recovery(DeviceState *deviceState, QObject *parent = nullptr);
    ~Recovery();

    DeviceState *deviceState() const;

    WirelessStatus wirelessStatus();

    bool exitRecoveryMode();
    bool setBootMode(BootMode mode);

    bool startFUS();
    bool startWirelessStack();
    bool deleteWirelessStack();
    bool upgradeWirelessStack();
    bool checkWirelessStack();

    bool downloadFirmware(QIODevice *file);
    bool downloadOptionBytes(QIODevice *file);
    bool downloadWirelessStack(QIODevice *file, uint32_t addr = 0);

private:
    DeviceState *m_deviceState;
};

}
}

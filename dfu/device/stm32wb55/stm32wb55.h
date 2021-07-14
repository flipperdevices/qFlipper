#pragma once

#include "dfusedevice.h"
#include "optionbytes.h"

namespace STM32WB55 {

class STM32WB55 : public DfuseDevice
{
    Q_OBJECT

public:
    enum class Partition {
        Flash = 0,
        OptionBytes,
        OTP
    };

    struct FUSState {
        enum Status {
            Idle = 0x00,
            FWUpgradeOngoing = 0x10,
            FUSUpgradeOngoing = 0x20,
            ServiceOngoing = 0x30,
            ErrorOccured = 0xFF,
            Invalid = 0x0BADF00D
        };

        enum Error {
            NoError = 0x00,
            ImageNotFound,
            ImageCorrupt,
            ImageNotAuthentic,
            NotEnoughSpace,
            UserAbort,
            EraseError,
            WriteError,
            STTagNotFound,
            CustomTagNotFound,
            AuthKeyLocked,
            RollBackError = 0x11,
            NotRunning = 0xFE,
            Unknown = 0xFF
        };

        FUSState() = default;
        FUSState(Status s, Error e):
            status(s),
            error(e)
        {}

        bool isValid() const { return status != Invalid; }

        Status status = Invalid;
        Error error = Unknown;
    };

    STM32WB55(const USBDeviceInfo &info, QObject *parent = nullptr);

    OptionBytes optionBytes();
    bool setOptionBytes(const OptionBytes &ob);

    QByteArray OTPData(qint64 len);

    FUSState FUSGetState();
    bool FUSFwDelete();
    bool FUSFwUpgrade();
    bool FUSStartWirelessStack();
};

}

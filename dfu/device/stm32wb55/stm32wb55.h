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

    struct FUSStatusType {
        enum State {
            Idle = 0x00,
            FWUpgradeOngoing = 0x10,
            FUSUpgradeOngoing = 0x20,
            ServiceOngoing = 0x30,
            ErrorOccured = 0xFF
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

        FUSStatusType() = default;
        FUSStatusType(int s, int e):
            state(s),
            error(e)
        {}

        int state = ErrorOccured;
        int error = Unknown;
    };

    STM32WB55(const USBDeviceInfo &info, QObject *parent = nullptr);

    OptionBytes optionBytes();
    bool setOptionBytes(const OptionBytes &ob);

    QByteArray OTPData(qint64 len);

    FUSStatusType FUSStatus();
    bool startFUS();
    bool startRadioStack();
};

}

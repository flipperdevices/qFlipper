#pragma once

#include "dfusedevice.h"
#include "versioninfo.h"
#include "optionbytes.h"
#include "fusstate.h"

namespace STM32 {

class STM32WB55 : public DfuseDevice
{
    Q_OBJECT

public:
    enum class Partition {
        Flash = 0,
        OptionBytes,
        OTP
    };

    STM32WB55(const USBDeviceInfo &info, QObject *parent = nullptr);

    WB55::OptionBytes optionBytes();
    bool setOptionBytes(const WB55::OptionBytes &ob);

    WB55::VersionInfo versionInfo();
    QByteArray OTPData(qint64 len);

    WB55::FUSState FUSGetState();
    bool FUSFwDelete();
    bool FUSFwUpgrade();
    bool FUSStartWirelessStack();
};

}

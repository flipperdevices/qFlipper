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

    STM32WB55(const USBDeviceInfo &info, QObject *parent = nullptr);

    OptionBytes optionBytes();
    bool setOptionBytes(const OptionBytes &ob);

    QByteArray otpData(qint64 len);
};

}

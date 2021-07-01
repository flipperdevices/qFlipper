#include "stm32wb55.h"

#include <QBuffer>

#include "macros.h"

#define OTP_ADDR 0x1FFF7000UL
#define OTP_MAX_SIZE 1024

#define OPTION_BYTES_ADDR 0x1FFF8000UL

namespace STM32WB55 {

STM32WB55::STM32WB55(const USBDeviceInfo &info, QObject *parent):
    DfuseDevice(info, parent)
{}

OptionBytes STM32WB55::optionBytes()
{
    QBuffer buf;
    check_return_val(buf.open(QIODevice::WriteOnly), "Failed to create buffer", QByteArray());

    upload(&buf, OPTION_BYTES_ADDR, OptionBytes::size(), (uint8_t)Partition::OptionBytes);
    buf.close();

    check_return_val(buf.bytesAvailable() == OptionBytes::size(), "Failed to read option bytes", QByteArray());
    return buf.data();
}

bool STM32WB55::setOptionBytes(const OptionBytes &ob)
{
    QBuffer buf;
    check_return_bool(buf.open(QIODevice::ReadWrite), "Failed to create buffer");

    const auto success = (buf.write(ob.data()) == OptionBytes::size()) && buf.reset();

    if(success) {
        info_msg("Please ignore the following error message, it is normal");
        download(&buf, OPTION_BYTES_ADDR, (uint8_t)Partition::OptionBytes);

    } else {
        error_msg("Failed to fill buffer");
        return false;
    }

    buf.close();
    return true;
}

QByteArray STM32WB55::otpData(qint64 len)
{
    len = qMin<qint64>(len, OTP_MAX_SIZE);

    QBuffer buf;
    check_return_val(buf.open(QIODevice::WriteOnly), "Failed to create buffer", QByteArray());

    const auto success = upload(&buf, OTP_ADDR, len, (uint8_t)Partition::OTP);
    buf.close();

    check_return_val(success && (buf.bytesAvailable() == len), "Failed to read option bytes", QByteArray());
    return buf.data();
}

}

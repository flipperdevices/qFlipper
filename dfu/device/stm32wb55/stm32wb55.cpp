#include "stm32wb55.h"

#include <QBuffer>

#include "debug.h"

#define OTP_ADDR 0x1FFF7000UL
#define OTP_MAX_SIZE 1024

#define OPTION_BYTES_ADDR 0x1FFF8000UL

#define FUS_STATUS_ADDR 0xFFFF0054UL
#define FUS_STATUS_SIZE 2

namespace STM32 {

using namespace WB55;

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
        begin_ignore_block();
        download(&buf, OPTION_BYTES_ADDR, (uint8_t)Partition::OptionBytes);
        end_ignore_block();

    } else {
        error_msg("Failed to fill buffer");
        return false;
    }

    buf.close();
    return true;
}

QByteArray STM32WB55::OTPData(qint64 len)
{
    len = qMin<qint64>(len, OTP_MAX_SIZE);

    QBuffer buf;
    check_return_val(buf.open(QIODevice::WriteOnly), "Failed to create buffer", QByteArray());

    const auto success = upload(&buf, OTP_ADDR, len, (uint8_t)Partition::OTP);
    buf.close();

    check_return_val(success && (buf.bytesAvailable() == len), "Failed to read option bytes", QByteArray());
    return buf.data();
}

FUSState STM32WB55::FUSGetState()
{
    QBuffer buf;
    check_return_val(buf.open(QIODevice::WriteOnly), "Failed to create buffer", FUSState());

    upload(&buf, FUS_STATUS_ADDR, FUS_STATUS_SIZE, (uint8_t)Partition::Flash);
    buf.close();

    check_return_val(buf.bytesAvailable() == FUS_STATUS_SIZE, "Failed to read FUS status", FUSState());

    // Casting to unsigned char to prevent sign extension
    return FUSState(static_cast<FUSState::Status>((unsigned char)buf.data().at(0)),
                    static_cast<FUSState::Error>((unsigned char)buf.data().at(1)));
}

bool STM32WB55::FUSFwDelete()
{
    const QByteArray data(1, 0x52);
    check_return_bool(download(data), "Failed to send FW_DELETE command");
    return true;
}

bool STM32WB55::FUSFwUpgrade()
{
    const QByteArray data(1, 0x53);
    check_return_bool(download(data), "Failed to send FW_UPGRADE command");
    return true;
}

bool STM32WB55::FUSStartWirelessStack()
{
    const QByteArray data(1, 0x5A);
    check_return_bool(download(data), "Failed to start wireless stack");
    return true;
}

}

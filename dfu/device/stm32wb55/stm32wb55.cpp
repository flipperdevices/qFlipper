#include "stm32wb55.h"

#include <QBuffer>
#include <QtEndian>

#include "debug.h"
#include "infotable.h"

#define OTP_ADDR 0x1FFF7000UL
#define OTP_MAX_SIZE 1024

#define OPTION_BYTES_ADDR 0x1FFF8000UL

#define FUS_STATUS_ADDR 0xFFFF0054UL
#define FUS_STATUS_SIZE 2

#define SRAM2A_BASE 0x20030000UL
#define DIT_FUS_MAGIC_NUMBER 0xa94656b

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

VersionInfo STM32WB55::versionInfo()
{
    QBuffer buf;
    VersionInfo ret;

    if(!buf.open(QIODevice::WriteOnly)) {
        qCDebug(CATEGORY_DEBUG) << "Failed to open a buffer for writing:" << buf.errorString();
        return ret;
    }

    auto success = upload(&buf, SRAM2A_BASE, sizeof(uint32_t), (uint8_t)Partition::Flash);
    if(!success || buf.data().size() != sizeof(uint32_t)) {
        qCDebug(CATEGORY_DEBUG) << "Failed to read device information table location";
        return ret;
    }

    const auto addr = qFromLittleEndian<uint32_t>(buf.data().data());
    if(addr < SRAM2A_BASE) {
        qCDebug(CATEGORY_DEBUG).noquote().nospace() << "Invalid address 0x" << QString::number(addr, 16);
        return ret;
    }

    buf.reset();

    const auto size = qMax(sizeof(FUSDeviceInfoTable), sizeof(DeviceInfoTable));

    qCDebug(CATEGORY_DEBUG).noquote().nospace() << "Reading " << size << " bytes from address 0x" << QString::number(addr, 16);

    // Not checking IPCCDBA since it's always 0
    success = upload(&buf, addr, size, (uint8_t)Partition::Flash);
    buf.close();

    if(!success || buf.data().size() != size) {
        qCDebug(CATEGORY_DEBUG) << "Failed to read device information table";
        return ret;
    }

    const auto *fusInfoTable = (const FUSDeviceInfoTable*)buf.data().data();

    if(fusInfoTable->magic == DIT_FUS_MAGIC_NUMBER) {
        ret.FUSVersion = QStringLiteral("%1.%2.%3")
                .arg(fusInfoTable->FUSVersion.major)
                .arg(fusInfoTable->FUSVersion.minor)
                .arg(fusInfoTable->FUSVersion.sub);

        ret.WirelessVersion = QStringLiteral("%1.%2.%3")
                .arg(fusInfoTable->wirelessStackVersion.major)
                .arg(fusInfoTable->wirelessStackVersion.minor)
                .arg(fusInfoTable->wirelessStackVersion.sub);
    } else {
        const auto *wirelessInfoTable = (const DeviceInfoTable*)buf.data().data();

        ret.FUSVersion = QStringLiteral("%1.%2.%3")
                .arg(wirelessInfoTable->FUS.version.major)
                .arg(wirelessInfoTable->FUS.version.minor)
                .arg(wirelessInfoTable->FUS.version.sub);

        ret.WirelessVersion = QStringLiteral("%1.%2.%3")
                .arg(wirelessInfoTable->WirelessStack.version.major)
                .arg(wirelessInfoTable->WirelessStack.version.minor)
                .arg(wirelessInfoTable->WirelessStack.version.sub);
    }

    return ret;
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

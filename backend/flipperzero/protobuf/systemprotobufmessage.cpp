#include "systemprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

SystemPingRequest::SystemPingRequest(QSerialPort *serialPort):
    AbstractMainProtobufRequest(serialPort)
{
    pbMessage()->content.system_ping_request = PB_System_PingRequest_init_default;
}

SystemPingResponse::SystemPingResponse(QSerialPort *serialPort):
    AbstractMainProtobufResponse(serialPort)
{}

SystemDeviceInfoRequest::SystemDeviceInfoRequest(QSerialPort *serialPort):
    AbstractMainProtobufRequest(serialPort)
{
    pbMessage()->content.system_device_info_request = PB_System_DeviceInfoRequest_init_default;
}

SystemDeviceInfoResponse::SystemDeviceInfoResponse(QSerialPort *serialPort):
    AbstractMainProtobufResponse(serialPort)
{}

const QByteArray SystemDeviceInfoResponse::key() const
{
    return QByteArray(pbMessage()->content.system_device_info_response.key);
}

const QByteArray SystemDeviceInfoResponse::value() const
{
    return QByteArray(pbMessage()->content.system_device_info_response.value);
}

SystemRebootRequest::SystemRebootRequest(QSerialPort *serialPort, PB_System_RebootRequest_RebootMode mode):
    AbstractMainProtobufRequest(serialPort)
{
    pbMessage()->content.system_reboot_request.mode = mode;
}

SystemFactoryResetRequest::SystemFactoryResetRequest(QSerialPort *serialPort):
    AbstractMainProtobufRequest(serialPort)
{
    pbMessage()->content.system_factory_reset_request = PB_System_FactoryResetRequest_init_default;
}

SystemGetDateTimeRequest::SystemGetDateTimeRequest(QSerialPort *serialPort):
    AbstractMainProtobufRequest(serialPort)
{
    pbMessage()->content.system_get_datetime_request = PB_System_GetDateTimeRequest_init_default;
}

SystemGetDateTimeResponse::SystemGetDateTimeResponse(QSerialPort *serialPort):
    AbstractMainProtobufResponse(serialPort)
{}

QDateTime SystemGetDateTimeResponse::dateTime() const
{
    if(!pbMessage()->content.system_get_datetime_response.has_datetime) {
        return QDateTime();
    }

    const auto &ts = pbMessage()->content.system_get_datetime_response.datetime;

    const QDate date(ts.year, ts.month, ts.day);
    const QTime time(ts.hour, ts.minute, ts.second);

    return QDateTime(date, time);
}

SystemSetDateTimeRequest::SystemSetDateTimeRequest(QSerialPort *serialPort, const QDateTime &dateTime):
    AbstractMainProtobufRequest(serialPort)
{
    pbMessage()->content.system_set_datetime_request.has_datetime = true;
    auto &ts = pbMessage()->content.system_get_datetime_response.datetime;

    ts.day = dateTime.date().day();
    ts.month = dateTime.date().day();
    ts.weekday = dateTime.date().dayOfWeek();
    ts.year = dateTime.date().year();

    ts.hour = dateTime.time().hour();
    ts.minute = dateTime.time().minute();
    ts.second = dateTime.time().second();
}

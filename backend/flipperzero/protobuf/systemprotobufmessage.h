#pragma once

#include "mainprotobufmessage.h"

#include <QDateTime>

namespace Flipper {
namespace Zero {

class SystemPingRequest:
public AbstractMainProtobufRequest<PB_Main_system_ping_request_tag>
{
public:
    SystemPingRequest(QSerialPort *serialPort);
};

class SystemPingResponse:
public AbstractMainProtobufResponse<PB_Main_system_ping_response_tag>
{
public:
    SystemPingResponse(QSerialPort *serialPort);
};

class SystemDeviceInfoRequest:
public AbstractMainProtobufRequest<PB_Main_system_device_info_request_tag>
{
public:
    SystemDeviceInfoRequest(QSerialPort *serialPort);
};

class SystemDeviceInfoResponse:
public AbstractMainProtobufResponse<PB_Main_system_device_info_response_tag>
{
public:
    SystemDeviceInfoResponse(QSerialPort *serialPort);

    const QByteArray key() const;
    const QByteArray value() const;
};

class SystemRebootRequest:
public AbstractMainProtobufRequest<PB_Main_system_reboot_request_tag>
{
public:
    SystemRebootRequest(QSerialPort *serialPort, PB_System_RebootRequest_RebootMode mode);
};

class SystemFactoryResetRequest:
public AbstractMainProtobufRequest<PB_Main_system_factory_reset_request_tag>
{
public:
    SystemFactoryResetRequest(QSerialPort *serialPort);
};

class SystemGetDateTimeRequest:
public AbstractMainProtobufRequest<PB_Main_system_get_datetime_request_tag>
{
public:
    SystemGetDateTimeRequest(QSerialPort *serialPort);
};

class SystemGetDateTimeResponse:
public AbstractMainProtobufResponse<PB_Main_system_get_datetime_response_tag>
{
public:
    SystemGetDateTimeResponse(QSerialPort *serialPort);
    QDateTime dateTime() const;
};

class SystemSetDateTimeRequest:
public AbstractMainProtobufRequest<PB_Main_system_set_datetime_request_tag>
{
public:
    SystemSetDateTimeRequest(QSerialPort *serialPort, const QDateTime &dateTime);
};

}
}


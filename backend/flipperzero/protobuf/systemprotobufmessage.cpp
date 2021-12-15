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

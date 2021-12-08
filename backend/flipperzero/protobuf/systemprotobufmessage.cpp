#include "systemprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

SystemDeviceInfoRequest::SystemDeviceInfoRequest(QSerialPort *serialPort):
    AbstractMainProtobufRequest(serialPort)
{
    pbMessage()->which_content = PB_Main_system_device_info_request_tag;
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

bool SystemDeviceInfoResponse::isValidType() const
{
    return whichContent() == PB_Main_system_device_info_response_tag;
}

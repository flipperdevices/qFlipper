#pragma once

#include "mainprotobufmessage.h"

namespace Flipper {
namespace Zero {

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

}
}


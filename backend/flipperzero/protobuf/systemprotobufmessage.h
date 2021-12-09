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

}
}


#pragma once

#include "mainprotobufmessage.h"

namespace Flipper {
namespace Zero {

class SystemDeviceInfoRequest : public AbstractMainProtobufRequest
{
public:
    SystemDeviceInfoRequest(QSerialPort *serialPort);
};

class SystemDeviceInfoResponse : public AbstractMainProtobufResponse
{
public:
    SystemDeviceInfoResponse(QSerialPort *serialPort);
    const QByteArray key() const;
    const QByteArray value() const;

    bool isValidType() const override;
};

}
}


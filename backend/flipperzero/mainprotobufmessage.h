#pragma once

#include "abstractprotobufmessage.h"

#include "protobuf/flipper.pb.h"

namespace Flipper {
namespace Zero {

class AbstractMainProtobufRequest : public AbstractProtobufRequest<PB_Main>
{
public:
    AbstractMainProtobufRequest(QSerialPort *serialPort);
};

class AbstractMainProtobufResponse : public AbstractProtobufResponse<PB_Main>
{
public:
    AbstractMainProtobufResponse(QSerialPort *serialPort);
    virtual ~AbstractMainProtobufResponse();

    bool hasNext() const;
    PB_CommandStatus commandStatus() const;
    quint32 whichContent() const;

    bool isOk() const;
    virtual bool isValidType() const = 0;
};

class EmptyResponse : public AbstractMainProtobufResponse
{
public:
    EmptyResponse(QSerialPort *serialPort);
    bool isValidType() const override;
};

class GuiStartScreenStreamRequest : public AbstractMainProtobufRequest
{
public:
    GuiStartScreenStreamRequest(QSerialPort *serialPort);
};

class GuiScreenFrameResponse : public AbstractMainProtobufResponse
{
public:
    GuiScreenFrameResponse(QSerialPort *serialPort);
    const QByteArray screenFrame() const;

    bool isValidType() const override;
};

class GuiSendInputRequest : public AbstractMainProtobufRequest
{
public:
    GuiSendInputRequest(QSerialPort *serialPort);
};

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


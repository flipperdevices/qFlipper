#pragma once

#include "abstractprotobufmessage.h"
#include "messages/flipper.pb.h"

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

class MainStopSessionRequest : public AbstractMainProtobufRequest
{
public:
    MainStopSessionRequest(QSerialPort *serialPort);
};

class MainEmptyResponse : public AbstractMainProtobufResponse
{
public:
    MainEmptyResponse(QSerialPort *serialPort);
    bool isValidType() const override;
};

}
}


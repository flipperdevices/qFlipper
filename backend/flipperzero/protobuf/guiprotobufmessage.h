#pragma once

#include "mainprotobufmessage.h"

namespace Flipper {
namespace Zero {

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

}
}


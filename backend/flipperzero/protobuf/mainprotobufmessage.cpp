#include "mainprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

AbstractMainProtobufRequest::AbstractMainProtobufRequest(QSerialPort *serialPort):
    AbstractProtobufRequest<PB_Main>(&PB_Main_msg, serialPort)
{}

AbstractMainProtobufResponse::AbstractMainProtobufResponse(QSerialPort *serialPort):
    AbstractProtobufResponse<PB_Main>(&PB_Main_msg, serialPort)
{}

AbstractMainProtobufResponse::~AbstractMainProtobufResponse()
{}

bool AbstractMainProtobufResponse::hasNext() const
{
    return pbMessage()->has_next;
}

PB_CommandStatus AbstractMainProtobufResponse::commandStatus() const
{
    return pbMessage()->command_status;
}

quint32 AbstractMainProtobufResponse::whichContent() const
{
    return pbMessage()->which_content;
}

bool AbstractMainProtobufResponse::isOk() const
{
    return commandStatus() == PB_CommandStatus_OK;
}

MainEmptyResponse::MainEmptyResponse(QSerialPort *serialPort):
    AbstractMainProtobufResponse(serialPort)
{}

bool MainEmptyResponse::isValidType() const
{
    return whichContent() == PB_Main_empty_tag;
}

MainStopSessionRequest::MainStopSessionRequest(QSerialPort *serialPort):
    AbstractMainProtobufRequest(serialPort)
{
    pbMessage()->which_content = PB_Main_stop_session_tag;
    pbMessage()->content.stop_session = PB_StopSession_init_default;
}

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

EmptyResponse::EmptyResponse(QSerialPort *serialPort):
    AbstractMainProtobufResponse(serialPort)
{}

bool EmptyResponse::isValidType() const
{
    return whichContent() == PB_Main_empty_tag;
}

GuiScreenFrameResponse::GuiScreenFrameResponse(QSerialPort *serialPort):
    AbstractMainProtobufResponse(serialPort)
{}

const QByteArray GuiScreenFrameResponse::screenFrame() const
{
    const auto *data = pbMessage()->content.gui_screen_frame.data;
    return QByteArray((const char*)data->bytes, data->size);
}

bool GuiScreenFrameResponse::isValidType() const
{
    return whichContent() == PB_Main_gui_screen_frame_tag;
}

GuiStartScreenStreamRequest::GuiStartScreenStreamRequest(QSerialPort *serialPort):
    AbstractMainProtobufRequest(serialPort)
{
    pbMessage()->which_content = PB_Main_gui_start_screen_stream_request_tag;
    pbMessage()->content.gui_start_screen_stream_request = PB_Gui_StartScreenStreamRequest_init_default;
}


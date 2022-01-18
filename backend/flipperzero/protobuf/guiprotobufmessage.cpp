#include "guiprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

GuiStartScreenStreamRequest::GuiStartScreenStreamRequest(QSerialPort *serialPort):
    AbstractMainProtobufRequest(serialPort)
{
    pbMessage()->content.gui_start_screen_stream_request = PB_Gui_StartScreenStreamRequest_init_default;
}

GuiStopScreenStreamRequest::GuiStopScreenStreamRequest(QSerialPort *serialPort):
    AbstractMainProtobufRequest(serialPort)
{
    pbMessage()->content.gui_start_screen_stream_request = PB_Gui_StopScreenStreamRequest_init_default;
}

GuiScreenFrameResponse::GuiScreenFrameResponse(QSerialPort *serialPort):
    AbstractMainProtobufResponse(serialPort)
{}

const QByteArray GuiScreenFrameResponse::screenFrame() const
{
    const auto *data = pbMessage()->content.gui_screen_frame.data;
    return QByteArray((const char*)data->bytes, data->size);
}

GuiSendInputRequest::GuiSendInputRequest(QSerialPort *serialPort, PB_Gui_InputKey key, PB_Gui_InputType type):
    AbstractMainProtobufRequest(serialPort)
{
    pbMessage()->content.gui_send_input_event_request.key = key;
    pbMessage()->content.gui_send_input_event_request.type = type;
}

GuiStartVirtualDisplayRequest::GuiStartVirtualDisplayRequest(QSerialPort *serialPort, const QByteArray &screenData):
    AbstractMainProtobufRequest(serialPort)
{
    auto &request = pbMessage()->content.gui_start_virtual_display_request;

    request = PB_Gui_StartVirtualDisplayRequest_init_default;
    request.has_first_frame = !screenData.isEmpty();

    if(request.has_first_frame) {
        request.first_frame.data = (pb_bytes_array_t*)malloc(PB_BYTES_ARRAY_T_ALLOCSIZE(screenData.size()));
        request.first_frame.data->size = screenData.size();
        memcpy(request.first_frame.data->bytes, screenData.data(), screenData.size());
    }
}

GuiStopVirtualDisplayRequest::GuiStopVirtualDisplayRequest(QSerialPort *serialPort):
    AbstractMainProtobufRequest(serialPort)
{
    pbMessage()->content.gui_stop_virtual_display_request = PB_Gui_StopVirtualDisplayRequest_init_default;
}

GuiScreenFrameRequest::GuiScreenFrameRequest(QSerialPort *serialPort, const QByteArray &screenData):
    AbstractMainProtobufRequest(serialPort)
{
    auto &request = pbMessage()->content.gui_screen_frame;

    request.data = (pb_bytes_array_t*)malloc(PB_BYTES_ARRAY_T_ALLOCSIZE(screenData.size()));
    request.data->size = screenData.size();
    memcpy(request.data->bytes, screenData.data(), screenData.size());
}

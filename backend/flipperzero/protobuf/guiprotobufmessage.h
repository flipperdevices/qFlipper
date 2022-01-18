#pragma once

#include "mainprotobufmessage.h"

namespace Flipper {
namespace Zero {

class GuiStartScreenStreamRequest:
public AbstractMainProtobufRequest<PB_Main_gui_start_screen_stream_request_tag>
{
public:
    GuiStartScreenStreamRequest(QSerialPort *serialPort);
};

class GuiStopScreenStreamRequest:
public AbstractMainProtobufRequest<PB_Main_gui_stop_screen_stream_request_tag>
{
public:
    GuiStopScreenStreamRequest(QSerialPort *serialPort);
};

class GuiScreenFrameRequest:
public AbstractMainProtobufRequest<PB_Main_gui_screen_frame_tag>
{
public:
    GuiScreenFrameRequest(QSerialPort *serialPort, const QByteArray &screenData);
};

class GuiScreenFrameResponse:
public AbstractMainProtobufResponse<PB_Main_gui_screen_frame_tag>
{
public:
    GuiScreenFrameResponse(QSerialPort *serialPort);
    const QByteArray screenFrame() const;
};

class GuiSendInputRequest:
public AbstractMainProtobufRequest<PB_Main_gui_send_input_event_request_tag>
{
public:
    GuiSendInputRequest(QSerialPort *serialPort, PB_Gui_InputKey key, PB_Gui_InputType type);
};

class GuiStartVirtualDisplayRequest:
public AbstractMainProtobufRequest<PB_Main_gui_start_virtual_display_request_tag>
{
public:
    GuiStartVirtualDisplayRequest(QSerialPort *serialPort, const QByteArray &screenData);
};

class GuiStopVirtualDisplayRequest:
public AbstractMainProtobufRequest<PB_Main_gui_stop_virtual_display_request_tag>
{
public:
    GuiStopVirtualDisplayRequest(QSerialPort *serialPort);
};

}
}


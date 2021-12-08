#include "guiprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

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

#include "guiprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

GuiStartScreenStreamRequest::GuiStartScreenStreamRequest(QSerialPort *serialPort):
    AbstractMainProtobufRequest(serialPort)
{
    pbMessage()->content.gui_start_screen_stream_request = PB_Gui_StartScreenStreamRequest_init_default;
}

GuiScreenFrameResponse::GuiScreenFrameResponse(QSerialPort *serialPort):
    AbstractMainProtobufResponse(serialPort)
{}

const QByteArray GuiScreenFrameResponse::screenFrame() const
{
    const auto *data = pbMessage()->content.gui_screen_frame.data;
    return QByteArray((const char*)data->bytes, data->size);
}

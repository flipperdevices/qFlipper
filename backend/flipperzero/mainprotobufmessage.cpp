#include "mainprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

MainProtobufMessage::MainProtobufMessage(QSerialPort *serialPort):
    AbstractProtobufMessage<PB_Main>(&PB_Main_msg, PB_Main_init_default, serialPort)
{}

//ProtobufMessage ProtobufMessage::startScreenStream()
//{
//}

bool MainProtobufMessage::hasNext() const
{
    return payload()->has_next;
}

uint32_t MainProtobufMessage::commandId() const
{
    return payload()->command_id;
}

pb_size_t MainProtobufMessage::whichContent() const
{
    return payload()->which_content;
}

PB_CommandStatus MainProtobufMessage::commandStatus() const
{
    return payload()->command_status;
}

const QByteArray MainProtobufMessage::screenFrame() const
{
    const auto *data = payload()->content.gui_screen_frame.data;
    return QByteArray((const char*)data->bytes, data->size);
}

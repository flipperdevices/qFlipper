#include "protobufmessage.h"

using namespace Flipper;
using namespace Zero;

ProtobufMessage::ProtobufMessage(QSerialPort *serialPort):
    AbstractProtobufMessage<PB_Main>(&PB_Main_msg, PB_Main_init_default, serialPort)
{}

//ProtobufMessage ProtobufMessage::startScreenStream()
//{
//}

bool ProtobufMessage::hasNext() const
{
    return payload()->has_next;
}

uint32_t ProtobufMessage::commandId() const
{
    return payload()->command_id;
}

pb_size_t ProtobufMessage::whichContent() const
{
    return payload()->which_content;
}

PB_CommandStatus ProtobufMessage::commandStatus() const
{
    return payload()->command_status;
}

const QByteArray ProtobufMessage::screenFrame() const
{
    const auto *data = payload()->content.gui_screen_frame.data;
    return QByteArray((const char*)data->bytes, data->size);
}

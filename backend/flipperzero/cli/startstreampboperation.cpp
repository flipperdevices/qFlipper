#include "startstreampboperation.h"

#include <QDebug>
#include <QTimer>
#include <QSerialPort>

#include <pb_decode.h>

#include "flipperzero/protobuf/flipper.pb.h"

using namespace Flipper;
using namespace Zero;

StartStreamPBOperation::StartStreamPBOperation(QSerialPort *serialPort, QObject *parent):
    AbstractProtobufOperation(serialPort, parent)
{}

const QString StartStreamPBOperation::description() const
{
    return QStringLiteral("Start Screen Streaming (protobuf) @%1").arg(serialPort()->portName());
}

void StartStreamPBOperation::onSerialPortReadyRead()
{
    PB_Main msg = PB_Main_init_default;

    if(!receiveMessage(&PB_Main_msg, &msg)) {
        return;
    } else if(msg.command_status != PB_CommandStatus_OK) {
        finishWithError(QStringLiteral("Device replied with an error status"));
    } else if(msg.which_content != PB_Main_empty_tag) {
        finishWithError(QStringLiteral("Expected empty reply, got something else"));
    } else {
        finish();
    }

    // TODO: Find a way to make this automagically
    pb_release(&PB_Main_msg, &msg);
}

bool StartStreamPBOperation::begin()
{
    PB_Main msg = PB_Main_init_default;
    msg.which_content = PB_Main_gui_start_screen_stream_request_tag;
    msg.content.gui_start_screen_stream_request = PB_Gui_StartScreenStreamRequest_init_default;

    return sendMessage(&PB_Main_msg, &msg);
}

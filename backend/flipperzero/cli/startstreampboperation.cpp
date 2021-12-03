#include "startstreampboperation.h"

#include <QDebug>
#include <QTimer>
#include <QSerialPort>

#include <pb_decode.h>
#include <pb_encode.h>

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
    m_receivedData.append(serialPort()->readAll());
    PB_Main message = PB_Main_init_default;
    message.which_content = PB_Main_empty_tag;

    pb_istream_t istream = PB_ISTREAM_EMPTY;
//    qDebug() << "AZAZA!" << serialPort()->readAll();
}

bool StartStreamPBOperation::begin()
{
    PB_Main message = PB_Main_init_default;
    message.which_content = PB_Main_gui_start_screen_stream_request_tag;
    message.content.gui_start_screen_stream_request = PB_Gui_StartScreenStreamRequest_init_default;

    pb_ostream_t ostream = PB_OSTREAM_SIZING;

    auto success = pb_encode_ex(&ostream, &PB_Main_msg, &message, PB_ENCODE_DELIMITED);
    if(!(success && ostream.bytes_written)) {
        setError(PB_GET_ERROR(&ostream));
        return false;
    }

    const QByteArray buf(ostream.bytes_written, 0);

    ostream = pb_ostream_from_buffer((unsigned char*)buf.data(), ostream.bytes_written);
    pb_encode_ex(&ostream, &PB_Main_msg, &message, PB_ENCODE_DELIMITED);

    success = (serialPort()->write(buf) == buf.size()) &&
               serialPort()->flush();

    if(!success) {
        setError(serialPort()->errorString());
    }

//    QTimer::singleShot(0, this, &AbstractOperation::finish);
    return success;
}

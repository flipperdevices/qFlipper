#include "abstractprotobufoperation.h"

#include <QDebug>
#include <QSerialPort>
#include <QLoggingCategory>

#include <pb_encode.h>
#include <pb_decode.h>

AbstractProtobufOperation::AbstractProtobufOperation(QSerialPort *serialPort, QObject *parent):
    AbstractSerialOperation(serialPort, parent)
{}

bool AbstractProtobufOperation::sendMessage(const pb_msgdesc_t *fields, void *msg)
{
    pb_ostream_t ostream {
        .callback = outputCallback,
        .state = serialPort(),
        .max_size = SIZE_MAX,
        .bytes_written = 0,
        .errmsg = nullptr
    };

    return pb_encode_ex(&ostream, fields, msg, PB_DECODE_DELIMITED) && serialPort()->flush();
}

bool AbstractProtobufOperation::receiveMessage(const pb_msgdesc_t *fields, void *msg)
{
    pb_istream_t istream {
        .callback = inputCallback,
        .state = serialPort(),
        .bytes_left = (size_t)serialPort()->bytesAvailable(),
        .errmsg = nullptr
    };

    serialPort()->startTransaction();
    const auto success = pb_decode_ex(&istream, fields, msg, PB_DECODE_DELIMITED);

    if(success) {
        serialPort()->commitTransaction();
    } else {
        serialPort()->rollbackTransaction();
    }

    return success;
}

bool AbstractProtobufOperation::inputCallback(pb_istream_t *stream, pb_byte_t *buf, size_t count)
{
    auto *serialPort = (QSerialPort*)stream->state;
    return serialPort->read((char*)buf, count) == (qint64)count;
}

bool AbstractProtobufOperation::outputCallback(pb_ostream_t *stream, const pb_byte_t *buf, size_t count)
{
    auto *serialPort = (QSerialPort*)stream->state;
    return serialPort->write((const char*)buf, count) == (qint64)count;
}

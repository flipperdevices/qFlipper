#pragma once

#include <QDebug>
#include <QSerialPort>

#include <pb.h>
#include <pb_encode.h>
#include <pb_decode.h>

#include "failable.h"

class QSerialPort;

template<typename Msg>
class AbstractProtobufMessage : public Failable
{
    enum class State {
        Ready,
        Sent,
        Received
    };

public:
    AbstractProtobufMessage(const pb_msgdesc_t *msgDesc, QSerialPort *serialPort);
    AbstractProtobufMessage(const pb_msgdesc_t *msgDesc, const Msg &msgInit, QSerialPort *serialPort);
    virtual ~AbstractProtobufMessage();

    bool send();
    bool receive();

protected:
    Msg *payload();
    const Msg *payload() const;

private:
    static bool inputCallback(pb_istream_t *stream, pb_byte_t *buf, size_t count);
    static bool outputCallback(pb_ostream_t *stream, const pb_byte_t *buf, size_t count);

    QSerialPort *m_serialPort;
    const pb_msgdesc_t *m_msgDesc;
    Msg m_message;
    State m_state;
};

template<typename Msg>
AbstractProtobufMessage<Msg>::AbstractProtobufMessage(const pb_msgdesc_t *msgDesc, QSerialPort *serialPort):
    m_serialPort(serialPort),
    m_msgDesc(msgDesc),
    m_state(State::Ready)
{
    memset(&m_message, 0, sizeof(Msg));
}

template<typename Msg>
AbstractProtobufMessage<Msg>::AbstractProtobufMessage(const pb_msgdesc_t *msgDesc, const Msg &msgInit, QSerialPort *serialPort):
    m_serialPort(serialPort),
    m_msgDesc(msgDesc),
    m_message(msgInit),
    m_state(State::Ready)
{}

template<typename Msg>
AbstractProtobufMessage<Msg>::~AbstractProtobufMessage()
{
    if(m_state == State::Received) {
        pb_release(m_msgDesc, &m_message);
    }
}

template<typename Msg>
bool AbstractProtobufMessage<Msg>::send()
{
    pb_ostream_t ostream {
        .callback = outputCallback,
                .state = m_serialPort,
                .max_size = SIZE_MAX,
                .bytes_written = 0,
                .errmsg = nullptr
    };

    const auto success = pb_encode_ex(&ostream, m_msgDesc, &m_message, PB_DECODE_DELIMITED) && m_serialPort->flush();
    if(!success) {
        setError(PB_GET_ERROR(&ostream));
    } else {
        m_state = State::Sent;
    }

    return success;
}

template<typename Msg>
bool AbstractProtobufMessage<Msg>::receive()
{
    pb_istream_t istream {
        .callback = inputCallback,
                .state = m_serialPort,
                .bytes_left = (size_t)m_serialPort->bytesAvailable(),
                .errmsg = nullptr
    };

    m_serialPort->startTransaction();
    const auto success = pb_decode_ex(&istream, m_msgDesc, &m_message, PB_DECODE_DELIMITED);

    if(success) {
        m_serialPort->commitTransaction();
        m_state = State::Received;
    } else {
        m_serialPort->rollbackTransaction();
        setError(PB_GET_ERROR(&istream));
    }

    return success;
}

template<typename Msg>
Msg *AbstractProtobufMessage<Msg>::payload()
{
    if(m_state == State::Received) {
        qDebug() << "Warning: potential modification of payload after it has been received";
    }

    return &m_message;
}

template<typename Msg>
const Msg *AbstractProtobufMessage<Msg>::payload() const
{
    return &m_message;
}

template<typename Msg>
bool AbstractProtobufMessage<Msg>::inputCallback(pb_istream_t *stream, pb_byte_t *buf, size_t count)
{
    auto *serialPort = (QSerialPort*)stream->state;
    return serialPort->read((char*)buf, count) == (qint64)count;
}

template<typename Msg>
bool AbstractProtobufMessage<Msg>::outputCallback(pb_ostream_t *stream, const pb_byte_t *buf, size_t count)
{
    auto *serialPort = (QSerialPort*)stream->state;
    return serialPort->write((const char*)buf, count) == (qint64)count;
}


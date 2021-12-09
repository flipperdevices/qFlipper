#pragma once

#include <QSerialPort>

#include <pb.h>
#include <pb_encode.h>
#include <pb_decode.h>

#include "failable.h"

template<typename Msg>
class AbstractProtobufMessage : public Failable
{
public:
    AbstractProtobufMessage(QSerialPort *serialPort);
    AbstractProtobufMessage(const Msg &msgInit, QSerialPort *serialPort);
    virtual ~AbstractProtobufMessage() {}

protected:
    QSerialPort *serialPort() const;

    Msg *pbMessage();
    const Msg *pbMessage() const;

private:
    QSerialPort *m_serialPort;
    Msg m_message;
};

template<const pb_msgdesc_t *MsgDesc, typename Msg>
class AbstractProtobufRequest : public AbstractProtobufMessage<Msg>
{
public:
    AbstractProtobufRequest(QSerialPort *serialPort);
    AbstractProtobufRequest(const Msg &msgInit, QSerialPort *serialPort);
    virtual ~AbstractProtobufRequest() {}

    bool send();

private:
    static bool outputCallback(pb_ostream_t *stream, const pb_byte_t *buf, size_t count);
};

template<const pb_msgdesc_t *MsgDesc, typename Msg>
class AbstractProtobufResponse : public AbstractProtobufMessage<Msg>
{
public:
    AbstractProtobufResponse(QSerialPort *serialPort);
    virtual ~AbstractProtobufResponse();

    bool receive();
    bool isComplete() const;

private:
    static bool inputCallback(pb_istream_t *stream, pb_byte_t *buf, size_t count);
    bool m_isComplete;
};

template<typename Msg>
AbstractProtobufMessage<Msg>::AbstractProtobufMessage(QSerialPort *serialPort):
    m_serialPort(serialPort)
{
    memset(&m_message, 0, sizeof(Msg));
}

template<typename Msg>
AbstractProtobufMessage<Msg>::AbstractProtobufMessage(const Msg &msgInit, QSerialPort *serialPort):
    m_serialPort(serialPort),
    m_message(msgInit)
{}

template<typename Msg>
QSerialPort *AbstractProtobufMessage<Msg>::serialPort() const
{
    return m_serialPort;
}

template<typename Msg>
Msg *AbstractProtobufMessage<Msg>::pbMessage()
{
    return &m_message;
}

template<typename Msg>
const Msg *AbstractProtobufMessage<Msg>::pbMessage() const
{
    return &m_message;
}

template<const pb_msgdesc_t *MsgDesc, typename Msg>
AbstractProtobufRequest<MsgDesc, Msg>::AbstractProtobufRequest(QSerialPort *serialPort):
    AbstractProtobufMessage<Msg>(serialPort)
{}

template<const pb_msgdesc_t *MsgDesc, typename Msg>
AbstractProtobufRequest<MsgDesc, Msg>::AbstractProtobufRequest(const Msg &msgInit, QSerialPort *serialPort):
    AbstractProtobufMessage<Msg>(msgInit, serialPort)
{}

template<const pb_msgdesc_t *MsgDesc, typename Msg>
bool AbstractProtobufRequest<MsgDesc, Msg>::send()
{
    pb_ostream_t ostream {
        .callback = outputCallback,
                .state = AbstractProtobufMessage<Msg>::serialPort(),
                .max_size = SIZE_MAX,
                .bytes_written = 0,
                .errmsg = nullptr
    };

    const auto success = pb_encode_ex(&ostream, MsgDesc, AbstractProtobufMessage<Msg>::pbMessage(), PB_DECODE_DELIMITED) &&
                                                         AbstractProtobufMessage<Msg>::serialPort()->flush();
    if(!success) {
        Failable::setError(PB_GET_ERROR(&ostream));
    }

    return success;
}

template<const pb_msgdesc_t *MsgDesc, typename Msg>
bool AbstractProtobufRequest<MsgDesc, Msg>::outputCallback(pb_ostream_t *stream, const pb_byte_t *buf, size_t count)
{
    auto *serialPort = (QSerialPort*)stream->state;
    return serialPort->write((const char*)buf, count) == (qint64)count;
}

template<const pb_msgdesc_t *MsgDesc, typename Msg>
AbstractProtobufResponse<MsgDesc, Msg>::AbstractProtobufResponse(QSerialPort *serialPort):
    AbstractProtobufMessage<Msg> (serialPort),
    m_isComplete(false)
{}

template<const pb_msgdesc_t *MsgDesc, typename Msg>
AbstractProtobufResponse<MsgDesc, Msg>::~AbstractProtobufResponse()
{
    if(m_isComplete) {
        pb_release(MsgDesc, AbstractProtobufMessage<Msg>::pbMessage());
    }
}

template<const pb_msgdesc_t *MsgDesc, typename Msg>
bool AbstractProtobufResponse<MsgDesc, Msg>::receive()
{
    pb_istream_t istream {
        .callback = inputCallback,
        .state = AbstractProtobufMessage<Msg>::serialPort(),
        .bytes_left = (size_t)AbstractProtobufMessage<Msg>::serialPort()->bytesAvailable(),
        .errmsg = nullptr
    };

    AbstractProtobufMessage<Msg>::serialPort()->startTransaction();
    m_isComplete = pb_decode_ex(&istream, MsgDesc, AbstractProtobufMessage<Msg>::pbMessage(), PB_DECODE_DELIMITED);

    if(m_isComplete) {
        AbstractProtobufMessage<Msg>::serialPort()->commitTransaction();

    } else {
        AbstractProtobufMessage<Msg>::serialPort()->rollbackTransaction();
        Failable::setError(PB_GET_ERROR(&istream));
    }

    return m_isComplete;
}

template<const pb_msgdesc_t *MsgDesc, typename Msg>
bool AbstractProtobufResponse<MsgDesc, Msg>::isComplete() const
{
    return m_isComplete;
}

template<const pb_msgdesc_t *MsgDesc, typename Msg>
bool AbstractProtobufResponse<MsgDesc, Msg>::inputCallback(pb_istream_t *stream, pb_byte_t *buf, size_t count)
{
    auto *serialPort = (QSerialPort*)stream->state;
    return serialPort->read((char*)buf, count) == (qint64)count;
}

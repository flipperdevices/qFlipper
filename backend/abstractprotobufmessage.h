#pragma once

#include <QDebug>
#include <QSerialPort>

#include <pb.h>
#include <pb_encode.h>
#include <pb_decode.h>

#include "failable.h"

template<typename Msg>
class AbstractProtobufMessage : public Failable
{
public:
    AbstractProtobufMessage(const pb_msgdesc_t *msgDesc, QSerialPort *serialPort);
    AbstractProtobufMessage(const pb_msgdesc_t *msgDesc, const Msg &msgInit, QSerialPort *serialPort);
    virtual ~AbstractProtobufMessage() {}

protected:
    QSerialPort *serialPort() const;

    Msg *pbMessage();
    const Msg *pbMessage() const;
    const pb_msgdesc_t *pbMessageDesc() const;

private:
    QSerialPort *m_serialPort;
    const pb_msgdesc_t *m_msgDesc;
    Msg m_message;
};

template<typename Msg>
class AbstractProtobufRequest : public AbstractProtobufMessage<Msg>
{
public:
    AbstractProtobufRequest(const pb_msgdesc_t *msgDesc, QSerialPort *serialPort);
    AbstractProtobufRequest(const pb_msgdesc_t *msgDesc, const Msg &msgInit, QSerialPort *serialPort);
    virtual ~AbstractProtobufRequest() {}

    bool send();

private:
    static bool outputCallback(pb_ostream_t *stream, const pb_byte_t *buf, size_t count);

};

template<typename Msg>
class AbstractProtobufResponse : public AbstractProtobufMessage<Msg>
{
public:
    AbstractProtobufResponse(const pb_msgdesc_t *msgDesc, QSerialPort *serialPort);
    virtual ~AbstractProtobufResponse();

    bool receive();
    bool isComplete() const;

private:
    static bool inputCallback(pb_istream_t *stream, pb_byte_t *buf, size_t count);
    bool m_isComplete;
};

template<typename Msg>
AbstractProtobufMessage<Msg>::AbstractProtobufMessage(const pb_msgdesc_t *msgDesc, QSerialPort *serialPort):
    m_serialPort(serialPort),
    m_msgDesc(msgDesc)
{
    memset(&m_message, 0, sizeof(Msg));
}

template<typename Msg>
AbstractProtobufMessage<Msg>::AbstractProtobufMessage(const pb_msgdesc_t *msgDesc, const Msg &msgInit, QSerialPort *serialPort):
    m_serialPort(serialPort),
    m_msgDesc(msgDesc),
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

template<typename Msg>
const pb_msgdesc_t *AbstractProtobufMessage<Msg>::pbMessageDesc() const
{
    return m_msgDesc;
}

template<typename Msg>
AbstractProtobufRequest<Msg>::AbstractProtobufRequest(const pb_msgdesc_t *msgDesc, QSerialPort *serialPort):
    AbstractProtobufMessage<Msg> (msgDesc, serialPort)
{}

template<typename Msg>
AbstractProtobufRequest<Msg>::AbstractProtobufRequest(const pb_msgdesc_t *msgDesc, const Msg &msgInit, QSerialPort *serialPort):
    AbstractProtobufMessage<Msg> (msgDesc, msgInit, serialPort)
{}

template<typename Msg>
bool AbstractProtobufRequest<Msg>::send()
{
    pb_ostream_t ostream {
        .callback = outputCallback,
                .state = AbstractProtobufMessage<Msg>::serialPort(),
                .max_size = SIZE_MAX,
                .bytes_written = 0,
                .errmsg = nullptr
    };

    const auto success = pb_encode_ex(&ostream, AbstractProtobufMessage<Msg>::pbMessageDesc(),
                                                AbstractProtobufMessage<Msg>::pbMessage(), PB_DECODE_DELIMITED) &&
                                                AbstractProtobufMessage<Msg>::serialPort()->flush();
    if(!success) {
        Failable::setError(PB_GET_ERROR(&ostream));
    }

    return success;
}

template<typename Msg>
bool AbstractProtobufRequest<Msg>::outputCallback(pb_ostream_t *stream, const pb_byte_t *buf, size_t count)
{
    auto *serialPort = (QSerialPort*)stream->state;
    return serialPort->write((const char*)buf, count) == (qint64)count;
}

template<typename Msg>
AbstractProtobufResponse<Msg>::AbstractProtobufResponse(const pb_msgdesc_t *msgDesc, QSerialPort *serialPort):
    AbstractProtobufMessage<Msg> (msgDesc, serialPort),
    m_isComplete(false)
{}

template<typename Msg>
AbstractProtobufResponse<Msg>::~AbstractProtobufResponse()
{
    if(m_isComplete) {
        pb_release(AbstractProtobufMessage<Msg>::pbMessageDesc(),
                   AbstractProtobufMessage<Msg>::pbMessage());
    }
}

template<typename Msg>
bool AbstractProtobufResponse<Msg>::receive()
{
    pb_istream_t istream {
        .callback = inputCallback,
        .state = AbstractProtobufMessage<Msg>::serialPort(),
        .bytes_left = (size_t)this->serialPort()->bytesAvailable(),
        .errmsg = nullptr
    };

    AbstractProtobufMessage<Msg>::serialPort()->startTransaction();
    m_isComplete = pb_decode_ex(&istream, AbstractProtobufMessage<Msg>::pbMessageDesc(),
                                          AbstractProtobufMessage<Msg>::pbMessage(), PB_DECODE_DELIMITED);

    if(m_isComplete) {
        AbstractProtobufMessage<Msg>::serialPort()->commitTransaction();

    } else {
        AbstractProtobufMessage<Msg>::serialPort()->rollbackTransaction();
        Failable::setError(PB_GET_ERROR(&istream));
    }

    return m_isComplete;
}

template<typename Msg>
bool AbstractProtobufResponse<Msg>::isComplete() const
{
    return m_isComplete;
}

template<typename Msg>
bool AbstractProtobufResponse<Msg>::inputCallback(pb_istream_t *stream, pb_byte_t *buf, size_t count)
{
    auto *serialPort = (QSerialPort*)stream->state;
    return serialPort->read((char*)buf, count) == (qint64)count;
}

#pragma once

#include <QSerialPort>

#include <pb.h>
#include <pb_encode.h>
#include <pb_decode.h>

template<typename Msg>
class AbstractProtobufMessage
{
public:
    AbstractProtobufMessage(QSerialPort *serialPort);
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
    virtual ~AbstractProtobufRequest() {}

    bool send();
    qint64 bytesWritten() const;

private:
    static bool outputCallback(pb_ostream_t *stream, const pb_byte_t *buf, size_t count);
    qint64 m_bytesWritten;
};

template<const pb_msgdesc_t *MsgDesc, typename Msg>
class AbstractProtobufResponse : public AbstractProtobufMessage<Msg>
{
public:
    AbstractProtobufResponse(QSerialPort *serialPort);
    virtual ~AbstractProtobufResponse();

    bool receive();
    bool isComplete() const;
    void release();

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
    AbstractProtobufMessage<Msg>(serialPort),
    m_bytesWritten(0)
{}

template<const pb_msgdesc_t *MsgDesc, typename Msg>
bool AbstractProtobufRequest<MsgDesc, Msg>::send()
{
    auto *serialPort = AbstractProtobufMessage<Msg>::serialPort();

    pb_ostream_t ostream {
        outputCallback,
        serialPort,
        SIZE_MAX,
        0,
        nullptr
    };

    const auto success = pb_encode_ex(&ostream, MsgDesc, AbstractProtobufMessage<Msg>::pbMessage(),
                                      PB_ENCODE_DELIMITED) && serialPort->flush();

    m_bytesWritten = success ? ostream.bytes_written : -1;
    return success;
}

template<const pb_msgdesc_t *MsgDesc, typename Msg>
qint64 AbstractProtobufRequest<MsgDesc, Msg>::bytesWritten() const
{
    return m_bytesWritten;
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
    release();
}

template<const pb_msgdesc_t *MsgDesc, typename Msg>
bool AbstractProtobufResponse<MsgDesc, Msg>::receive()
{
    // Allow for re-use: release any previously allocated data
    release();

    auto *serialPort = AbstractProtobufMessage<Msg>::serialPort();

    pb_istream_t istream {
        inputCallback,
        serialPort,
        (size_t)serialPort->bytesAvailable(),
        nullptr
    };

    serialPort->startTransaction();

    m_isComplete = pb_decode_ex(&istream, MsgDesc, AbstractProtobufMessage<Msg>::pbMessage(), PB_DECODE_DELIMITED);

    if(m_isComplete) {
        serialPort->commitTransaction();
    } else {
        serialPort->rollbackTransaction();
    }

    return m_isComplete;
}

template<const pb_msgdesc_t *MsgDesc, typename Msg>
bool AbstractProtobufResponse<MsgDesc, Msg>::isComplete() const
{
    return m_isComplete;
}

template<const pb_msgdesc_t *MsgDesc, typename Msg>
void AbstractProtobufResponse<MsgDesc, Msg>::release()
{
    if(m_isComplete) {
        pb_release(MsgDesc, AbstractProtobufMessage<Msg>::pbMessage());
    }
}

template<const pb_msgdesc_t *MsgDesc, typename Msg>
bool AbstractProtobufResponse<MsgDesc, Msg>::inputCallback(pb_istream_t *stream, pb_byte_t *buf, size_t count)
{
    auto *serialPort = (QSerialPort*)stream->state;
    return serialPort->read((char*)buf, count) == (qint64)count;
}

#include "messagewrapper.h"

#include "pb_decode.h"

#include "mainresponse.h"

MessageWrapper::MessageWrapper(const QByteArray &buffer):
    m_message(PB_Main_init_zero)
{
    pb_istream_t s = pb_istream_from_buffer((const pb_byte_t*)buffer.data(), buffer.size());
    m_isComplete = pb_decode_ex(&s, &PB_Main_msg, &m_message, PB_DECODE_DELIMITED);
    m_encodedSize = buffer.size() - s.bytes_left;
}

MessageWrapper::MessageWrapper(MessageWrapper &&other):
    m_message(other.m_message),
    m_encodedSize(other.m_encodedSize),
    m_isComplete(other.m_isComplete)
{
    // Prevent potential double-free
    other.m_isComplete = false;
}

MessageWrapper::~MessageWrapper()
{
    if(m_isComplete) {
        pb_release(&PB_Main_msg, &m_message);
    }
}

const PB_Main &MessageWrapper::message() const
{
    return m_message;
}

size_t MessageWrapper::encodedSize() const
{
    return m_encodedSize;
}

bool MessageWrapper::isComplete() const
{
    return m_isComplete;
}

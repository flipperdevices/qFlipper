#include "mainrequest.h"

#include "pb_encode.h"

MainRequest::MainRequest(uint32_t id, pb_size_t tag, bool hasNext):
    m_message({id, PB_CommandStatus_OK, hasNext, {}, tag, {}})
{}

const QByteArray MainRequest::encode() const
{
    QByteArray ret;
    pb_ostream_t s = PB_OSTREAM_SIZING;

    if(!pb_encode_ex(&s, &PB_Main_msg, &m_message, PB_ENCODE_DELIMITED)) {
        return ret;
    }

    ret.resize((int)s.bytes_written);
    s = pb_ostream_from_buffer((pb_byte_t*)ret.data(), ret.size());

    if(!pb_encode_ex(&s, &PB_Main_msg, &m_message, PB_ENCODE_DELIMITED)) {
       ret.clear();
    }

    return ret;
}

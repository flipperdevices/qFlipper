#include "statusrequest.h"

#include "pb_encode.h"

StatusPingRequest::StatusPingRequest(uint32_t id, const QByteArray &data):
    MainRequest(id, PB_Main_system_ping_request_tag)
{
    if(data.isEmpty()) {
        return;
    }

    auto &content = m_message.content.system_ping_request;
    content.data = (pb_bytes_array_t*)malloc(PB_BYTES_ARRAY_T_ALLOCSIZE(data.size()));
    content.data->size = data.size();
    memcpy(content.data->bytes, data.data(), data.size());
}

StatusPingRequest::~StatusPingRequest()
{
    auto &content = m_message.content.system_ping_request;

    if(content.data) {
        free(content.data);
    }
}

#include "propertyrequest.h"

PropertyGetRequest::PropertyGetRequest(uint32_t id, const QByteArray &key):
    MainRequest(id, PB_Main_property_get_request_tag),
    m_key(key)
{
    auto &content = m_message.content.property_get_request;
    content.key = m_key.data();
}

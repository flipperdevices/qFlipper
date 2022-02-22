#include "statusresponse.h"

StatusPingResponse::StatusPingResponse(MessageWrapper &wrapper, QObject *parent):
    MainResponse(wrapper, parent)
{}

const QByteArray StatusPingResponse::data() const
{
    const auto *d = message().content.system_ping_response.data;
    return QByteArray((const char*)d->bytes, d->size);
}

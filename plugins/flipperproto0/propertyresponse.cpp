#include "propertyresponse.h"

PropertyGetResponse::PropertyGetResponse(MessageWrapper &wrapper, QObject *parent):
    MainResponse(wrapper, parent)
{}

const QByteArray PropertyGetResponse::key() const
{
    return message().content.property_get_response.key;
}

const QByteArray PropertyGetResponse::value() const
{
    return message().content.property_get_response.value;
}

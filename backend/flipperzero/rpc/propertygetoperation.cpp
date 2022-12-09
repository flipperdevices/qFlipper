#include "propertygetoperation.h"

#include "protobufplugininterface.h"
#include "mainresponseinterface.h"
#include "propertyresponseinterface.h"

using namespace Flipper;
using namespace Zero;

PropertyGetOperation::PropertyGetOperation(uint32_t id, const QByteArray &key, QObject *parent):
    AbstractProtobufOperation(id, parent),
    m_key(key)
{}

const QString PropertyGetOperation::description() const
{
    return QStringLiteral("Property Get");
}

const QByteArray PropertyGetOperation::value(const QByteArray &key) const
{
    return m_data.value(key);
}

const QByteArray PropertyGetOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    return encoder->propertyGet(id(), m_key);
}

bool PropertyGetOperation::processResponse(QObject *response)
{
    const auto *msg = qobject_cast<PropertyGetResponseInterface*>(response);

    if(msg) {
        m_data.insert(msg->key(), msg->value());
        return true;
    }

    return qobject_cast<EmptyResponseInterface*>(response);
}

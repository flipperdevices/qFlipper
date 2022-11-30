#include "systemprotobufversionoperation.h"

#include "protobufplugininterface.h"
#include "systemresponseinterface.h"

using namespace Flipper;
using namespace Zero;

SystemProtobufVersionOperation::SystemProtobufVersionOperation(uint32_t id, QObject *parent):
    AbstractProtobufOperation(id, parent),
    m_versionMajor(0),
    m_versionMinor(0)
{}

const QString SystemProtobufVersionOperation::description() const
{
    return QStringLiteral("System Protobuf Version");
}

uint32_t SystemProtobufVersionOperation::versionMajor() const
{
    return m_versionMajor;
}

uint32_t SystemProtobufVersionOperation::versionMinor() const
{
    return m_versionMinor;
}

const QByteArray SystemProtobufVersionOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    return encoder->systemProtobufVersion(id());
}

bool SystemProtobufVersionOperation::processResponse(QObject *response)
{
    auto *msg = qobject_cast<SystemProtobufVersionResponseInterface*>(response);

    if(msg) {
        m_versionMajor = msg->versionMajor();
        m_versionMinor = msg->versionMinor();
    }

    return msg;
}

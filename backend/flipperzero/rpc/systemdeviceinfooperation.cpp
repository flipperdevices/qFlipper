#include "systemdeviceinfooperation.h"

#include "protobufplugininterface.h"
#include "systemresponseinterface.h"

using namespace Flipper;
using namespace Zero;

SystemDeviceInfoOperation::SystemDeviceInfoOperation(uint32_t id, QObject *parent):
    AbstractProtobufOperation(id, parent)
{}

const QString SystemDeviceInfoOperation::description() const
{
    return QStringLiteral("System Device Info");
}

const QByteArray SystemDeviceInfoOperation::value(const QByteArray &key) const
{
    return m_data.value(key);
}

const QByteArray SystemDeviceInfoOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    return encoder->systemDeviceInfo(id());
}

bool SystemDeviceInfoOperation::processResponse(QObject *response)
{
    auto *msg = qobject_cast<SystemDeviceInfoResponseInterface*>(response);

    if(!msg) {
        return false;
    }

    m_data.insert(msg->key(), msg->value());
    return true;
}

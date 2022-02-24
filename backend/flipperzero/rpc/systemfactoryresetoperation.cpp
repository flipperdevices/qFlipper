#include "systemfactoryresetoperation.h"
#include "protobufplugininterface.h"

using namespace Flipper;
using namespace Zero;

SystemFactoryResetOperation::SystemFactoryResetOperation(uint32_t id, QObject *parent):
    AbstractProtobufOperation(id, parent)
{}

const QString SystemFactoryResetOperation::description() const
{
    return QStringLiteral("Factory Reset");
}

const QByteArray SystemFactoryResetOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    finishLater();
    return encoder->systemFactoryReset(id());
}

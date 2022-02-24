#include "systemrebootoperation.h"
#include "protobufplugininterface.h"

using namespace Flipper;
using namespace Zero;

SystemRebootOperation::SystemRebootOperation(uint32_t id, RebootMode rebootType, QObject *parent):
    AbstractProtobufOperation(id, parent),
    m_rebootMode(rebootType)
{}

const QString SystemRebootOperation::description() const
{
    return QStringLiteral("System Reboot");
}

const QByteArray SystemRebootOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    finishLater();
    return encoder->systemReboot(id(), (ProtobufPluginInterface::RebootMode)m_rebootMode);
}

#include "systemsetdatetimeoperation.h"

#include "protobufplugininterface.h"

using namespace Flipper;
using namespace Zero;

SystemSetDateTimeOperation::SystemSetDateTimeOperation(uint32_t id, const QDateTime &dateTime, QObject *parent):
    AbstractProtobufOperation(id, parent),
    m_dateTime(dateTime)
{}

const QString SystemSetDateTimeOperation::description() const
{
    return QStringLiteral("System Set DateTime");
}

const QByteArray SystemSetDateTimeOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    return encoder->systemSetDateTime(id(), m_dateTime);
}

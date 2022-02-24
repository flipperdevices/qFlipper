#include "systemgetdatetimeoperation.h"

#include "mainresponseinterface.h"
#include "systemresponseinterface.h"
#include "protobufplugininterface.h"

using namespace Flipper;
using namespace Zero;

SystemGetDateTimeOperation::SystemGetDateTimeOperation(uint32_t id, QObject *parent):
    AbstractProtobufOperation(id, parent)
{}

const QString SystemGetDateTimeOperation::description() const
{
    return QStringLiteral("System Get DateTime");
}

const QDateTime &SystemGetDateTimeOperation::dateTime() const
{
    return m_dateTime;
}

const QByteArray SystemGetDateTimeOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    return encoder->systemGetDateTime(id());
}

bool SystemGetDateTimeOperation::processResponse(QObject *response)
{
    auto *getDateTimeResponse = qobject_cast<SystemGetDateTimeResponseInterface*>(response);

    if(!getDateTimeResponse) {
        return false;
    }

    m_dateTime = getDateTimeResponse->dateTime();
    return true;
}

#include "systemupdateoperation.h"

#include "protobufplugininterface.h"
#include "systemresponseinterface.h"
#include "mainresponseinterface.h"

using namespace Flipper;
using namespace Zero;

SystemUpdateOperation::SystemUpdateOperation(uint32_t id, const QByteArray &manifestPath, QObject *parent):
    AbstractProtobufOperation(id, parent),
    m_manifestPath(manifestPath),
    m_isResultOk(false)
{}

const QString SystemUpdateOperation::description() const
{
    return QStringLiteral("System Update @%1").arg(QString(m_manifestPath));
}

const QString &SystemUpdateOperation::resultString() const
{
    return m_resultString;
}

bool SystemUpdateOperation::isResultOk() const
{
    return m_isResultOk;
}

const QByteArray SystemUpdateOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    return encoder->systemUpdateRequest(id(), m_manifestPath);
}

bool SystemUpdateOperation::processResponse(QObject *response)
{
    auto *updateResponse = qobject_cast<SystemUpdateResponseInterface*>(response);

    if(updateResponse) {
        m_isResultOk = updateResponse->isResultOk();
        m_resultString = updateResponse->resultString();
    }

    return true;
}

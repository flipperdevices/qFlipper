#include "guisendinputoperation.h"

#include "protobufplugininterface.h"

using namespace Flipper;
using namespace Zero;

GuiSendInputOperation::GuiSendInputOperation(uint32_t id, int key, int type, QObject *parent):
    AbstractProtobufOperation(id, parent),
    m_key(key),
    m_type(type)
{}

const QString GuiSendInputOperation::description() const
{
    return QStringLiteral("Gui Send Input");
}

const QByteArray GuiSendInputOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    return encoder->guiSendInput(id(), m_key, m_type);
}

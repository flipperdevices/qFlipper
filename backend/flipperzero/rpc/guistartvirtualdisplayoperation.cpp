#include "guistartvirtualdisplayoperation.h"

#include "protobufplugininterface.h"

using namespace Flipper;
using namespace Zero;

GuiStartVirtualDisplayOperation::GuiStartVirtualDisplayOperation(uint32_t id, const QByteArray &screenData, QObject *parent):
    AbstractProtobufOperation(id, parent),
    m_screenData(screenData)
{}

const QString GuiStartVirtualDisplayOperation::description() const
{
    return QStringLiteral("Gui Start VirtualDisplay");
}

const QByteArray GuiStartVirtualDisplayOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    return encoder->guiStartVirtualDisplay(id(), m_screenData);
}


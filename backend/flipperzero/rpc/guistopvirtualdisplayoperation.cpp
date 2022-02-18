#include "guistopvirtualdisplayoperation.h"

#include "protobufplugininterface.h"

using namespace Flipper;
using namespace Zero;

GuiStopVirtualDisplayOperation::GuiStopVirtualDisplayOperation(uint32_t id, QObject *parent):
    AbstractProtobufOperation(id, parent)
{}

const QString GuiStopVirtualDisplayOperation::description() const
{
    return QStringLiteral("Gui Stop VirtualDisplay");
}

const QByteArray GuiStopVirtualDisplayOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    return encoder->guiStopVirtualDisplay(id());
}

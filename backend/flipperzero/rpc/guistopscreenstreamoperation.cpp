#include "guistopscreenstreamoperation.h"

#include "protobufplugininterface.h"

using namespace Flipper;
using namespace Zero;

GuiStopScreenStreamOperation::GuiStopScreenStreamOperation(uint32_t id, QObject *parent):
    AbstractProtobufOperation(id, parent)
{}

const QString GuiStopScreenStreamOperation::description() const
{
    return QStringLiteral("Gui Stop ScreenStream");
}

const QByteArray GuiStopScreenStreamOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    return encoder->guiStopScreenStream(id());
}

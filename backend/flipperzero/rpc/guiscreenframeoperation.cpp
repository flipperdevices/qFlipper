#include "guiscreenframeoperation.h"
#include "protobufplugininterface.h"

using namespace Flipper;
using namespace Zero;

GuiScreenFrameOperation::GuiScreenFrameOperation(uint32_t id, const QByteArray &screenData, QObject *parent):
    AbstractProtobufOperation(id, parent),
    m_screenData(screenData)
{}

const QString GuiScreenFrameOperation::description() const
{
    return QStringLiteral("Gui ScreenFrame");
}

const QByteArray GuiScreenFrameOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    finishLater();
    return encoder->guiScreenFrame(id(), m_screenData);
}

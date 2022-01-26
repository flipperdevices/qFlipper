#include "guistartvirtualdisplayoperation.h"

#include <QSerialPort>

#include "flipperzero/protobuf/guiprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

GuiStartVirtualDisplayOperation::GuiStartVirtualDisplayOperation(QSerialPort *serialPort, const QByteArray &screenData, QObject *parent):
    AbstractProtobufOperation(serialPort, parent),
    m_screenData(screenData)
{}

const QString GuiStartVirtualDisplayOperation::description() const
{
    return QStringLiteral("Start virtual display @%1").arg(serialPort()->portName());
}

void GuiStartVirtualDisplayOperation::onSerialPortReadyRead()
{
    MainEmptyResponse response(serialPort());

    if(!response.receive()) {
        return;
    } else if(!response.isOk()) {
        finishWithError(BackendError::ProtocolError, QStringLiteral("Device replied with an error response: %1").arg(response.commandStatusString()));
    } else if(!response.isValidType()) {
        finishWithError(BackendError::ProtocolError, QStringLiteral("Expected empty reply, got something else"));
    } else {
        finish();
    }
}

bool GuiStartVirtualDisplayOperation::begin()
{
    GuiStartVirtualDisplayRequest request(serialPort(), m_screenData);
    return request.send();
}

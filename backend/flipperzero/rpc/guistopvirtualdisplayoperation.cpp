#include "guistopvirtualdisplayoperation.h"

#include <QSerialPort>

#include "flipperzero/protobuf/guiprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

GuiStopVirtualDisplayOperation::GuiStopVirtualDisplayOperation(QSerialPort *serialPort, QObject *parent):
    AbstractSerialOperation(serialPort, parent)
{}

const QString GuiStopVirtualDisplayOperation::description() const
{
    return QStringLiteral("Stop virtual display @%1").arg(serialPort()->portName());
}

void GuiStopVirtualDisplayOperation::onSerialPortReadyRead()
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

bool GuiStopVirtualDisplayOperation::begin()
{
    GuiStopVirtualDisplayRequest request(serialPort());
    return request.send();
}

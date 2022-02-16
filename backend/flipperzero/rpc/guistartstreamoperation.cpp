#include "guistartstreamoperation.h"

#include <QSerialPort>

#include "flipperzero/protobuf/guiprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

GuiStartStreamOperation::GuiStartStreamOperation(QSerialPort *serialPort, QObject *parent):
    AbstractSerialOperation(serialPort, parent)
{}

const QString GuiStartStreamOperation::description() const
{
    return QStringLiteral("Start screen streaming @%1").arg(serialPort()->portName());
}

void GuiStartStreamOperation::onSerialPortReadyRead()
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

bool GuiStartStreamOperation::begin()
{
    GuiStartScreenStreamRequest request(serialPort());
    return request.send();
}

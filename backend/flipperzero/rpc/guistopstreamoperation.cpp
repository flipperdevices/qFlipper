#include "guistopstreamoperation.h"

#include <QSerialPort>

#include "flipperzero/protobuf/guiprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

GuiStopStreamOperation::GuiStopStreamOperation(QSerialPort *serialPort, QObject *parent):
    AbstractProtobufOperation(serialPort, parent)
{}

const QString GuiStopStreamOperation::description() const
{
    return QStringLiteral("Stop screen streaming @%1").arg(serialPort()->portName());
}

void GuiStopStreamOperation::onSerialPortReadyRead()
{
    MainEmptyResponse response(serialPort());

    while(response.receive()) {
        if(!response.isOk()) {
            finishWithError(QStringLiteral("Device replied with an error response: %1").arg(response.commandStatusString()));

        } else if(!response.isValidType()) {
            if(response.whichContent() != GuiScreenFrameResponse::tag()) {
                finishWithError(QStringLiteral("Expected empty or screen frame reply, got something else"));
            } else {
                continue;
            }

        } else {
            finish();
        }

        return;
    }
}

bool GuiStopStreamOperation::begin()
{
    GuiStopScreenStreamRequest request(serialPort());
    return request.send();
}

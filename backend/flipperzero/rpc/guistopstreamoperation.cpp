#include "guistopstreamoperation.h"

#include <QSerialPort>

using namespace Flipper;
using namespace Zero;

GuiStopStreamOperation::GuiStopStreamOperation(QSerialPort *serialPort, QObject *parent):
    AbstractSerialOperation(serialPort, parent)
{}

const QString GuiStopStreamOperation::description() const
{
    return QStringLiteral("Stop screen streaming @%1").arg(serialPort()->portName());
}

void GuiStopStreamOperation::onSerialPortReadyRead()
{
}

bool GuiStopStreamOperation::begin()
{
    return false;
}

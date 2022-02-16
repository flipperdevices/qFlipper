#include "guistopvirtualdisplayoperation.h"

#include <QSerialPort>

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
}

bool GuiStopVirtualDisplayOperation::begin()
{
    return false;
}

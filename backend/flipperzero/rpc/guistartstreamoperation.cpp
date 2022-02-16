#include "guistartstreamoperation.h"

#include <QSerialPort>

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
{}

bool GuiStartStreamOperation::begin()
{
    return false;
}

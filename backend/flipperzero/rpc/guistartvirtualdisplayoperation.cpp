#include "guistartvirtualdisplayoperation.h"

#include <QSerialPort>

using namespace Flipper;
using namespace Zero;

GuiStartVirtualDisplayOperation::GuiStartVirtualDisplayOperation(QSerialPort *serialPort, const QByteArray &screenData, QObject *parent):
    AbstractSerialOperation(serialPort, parent),
    m_screenData(screenData)
{}

const QString GuiStartVirtualDisplayOperation::description() const
{
    return QStringLiteral("Start virtual display @%1").arg(serialPort()->portName());
}

void GuiStartVirtualDisplayOperation::onSerialPortReadyRead()
{
}

bool GuiStartVirtualDisplayOperation::begin()
{
    return false;
}

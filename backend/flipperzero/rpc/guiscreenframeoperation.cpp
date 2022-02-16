#include "guiscreenframeoperation.h"

#include <QSerialPort>

using namespace Flipper;
using namespace Zero;

GuiScreenFrameOperation::GuiScreenFrameOperation(QSerialPort *serialPort, const QByteArray &screenData, QObject *parent):
    AbstractSerialOperation(serialPort, parent),
    m_screenData(screenData)
{}

const QString GuiScreenFrameOperation::description() const
{
    return QStringLiteral("Send screen frame @%1").arg(serialPort()->portName());
}

bool GuiScreenFrameOperation::begin()
{
    return false;
}

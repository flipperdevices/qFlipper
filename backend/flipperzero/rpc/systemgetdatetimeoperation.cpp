#include "systemgetdatetimeoperation.h"

#include <QSerialPort>

using namespace Flipper;
using namespace Zero;

SystemGetDateTimeOperation::SystemGetDateTimeOperation(QSerialPort *serialPort, QObject *parent):
    AbstractSerialOperation(serialPort, parent)
{}

const QString SystemGetDateTimeOperation::description() const
{
    return QStringLiteral("Get DateTime @%1").arg(QString(serialPort()->portName()));
}

const QDateTime &SystemGetDateTimeOperation::dateTime() const
{
    return m_dateTime;
}

void SystemGetDateTimeOperation::onSerialPortReadyRead()
{
}

bool SystemGetDateTimeOperation::begin()
{
    return false;
}

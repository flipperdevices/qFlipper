#include "systemsetdatetimeoperation.h"

#include <QSerialPort>

using namespace Flipper;
using namespace Zero;

SystemSetDateTimeOperation::SystemSetDateTimeOperation(QSerialPort *serialPort, const QDateTime &dateTime, QObject *parent):
    AbstractSerialOperation(serialPort, parent),
    m_dateTime(dateTime)
{}

const QString SystemSetDateTimeOperation::description() const
{
    return QStringLiteral("Set DateTime @%1").arg(QString(serialPort()->portName()));
}

void SystemSetDateTimeOperation::onSerialPortReadyRead()
{
}

bool SystemSetDateTimeOperation::begin()
{
    return false;
}

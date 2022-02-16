#include "systemdeviceinfooperation.h"

#include <QSerialPort>

using namespace Flipper;
using namespace Zero;

SystemDeviceInfoOperation::SystemDeviceInfoOperation(QSerialPort *serialPort, QObject *parent):
    AbstractSerialOperation(serialPort, parent)
{}

const QString SystemDeviceInfoOperation::description() const
{
    return QStringLiteral("Device Info @%1").arg(QString(serialPort()->portName()));
}

const QByteArray SystemDeviceInfoOperation::result(const QByteArray &key) const
{
    return m_data.value(key);
}

void SystemDeviceInfoOperation::onSerialPortReadyRead()
{
}

bool SystemDeviceInfoOperation::begin()
{
    return false;
}

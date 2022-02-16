#include "systemfactoryresetoperation.h"

#include <QSerialPort>

using namespace Flipper;
using namespace Zero;

SystemFactoryResetOperation::SystemFactoryResetOperation(QSerialPort *serialPort, QObject *parent):
    AbstractSerialOperation(serialPort, parent),
    m_byteCount(0)
{}

const QString SystemFactoryResetOperation::description() const
{
    return QStringLiteral("Factory reset @%1").arg(QString(serialPort()->portName()));
}

void SystemFactoryResetOperation::onTotalBytesWrittenChanged()
{
    if(m_byteCount == totalBytesWritten()) {
        finish();
    }
}

bool SystemFactoryResetOperation::begin()
{
    return false;
}

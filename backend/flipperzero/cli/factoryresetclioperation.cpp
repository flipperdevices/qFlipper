#include "factoryresetclioperation.h"

#include <QSerialPort>

using namespace Flipper;
using namespace Zero;

FactoryResetCliOperation::FactoryResetCliOperation(QSerialPort *serialPort, QObject *parent):
    SimpleSerialOperation(serialPort, parent)
{}

const QString FactoryResetCliOperation::description() const
{
    return QStringLiteral("Factory reset @%1").arg(QString(serialPort()->portName()));
}

QByteArray FactoryResetCliOperation::endOfMessageToken() const
{
    return QByteArrayLiteral("Are you sure (y/n)?\r\n");
}

QByteArray FactoryResetCliOperation::commandLine() const
{
    return QByteArrayLiteral("factory_reset\r");
}

bool FactoryResetCliOperation::parseReceivedData()
{
    return (serialPort()->write("y\r\n") > 0) && serialPort()->flush();
}

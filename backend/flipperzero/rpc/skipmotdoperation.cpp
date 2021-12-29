#include "skipmotdoperation.h"

#include <QSerialPort>

using namespace Flipper;
using namespace Zero;

SkipMOTDOperation::SkipMOTDOperation(QSerialPort *serialPort, QObject *parent):
    SimpleSerialOperation(serialPort, parent)
{}

const QString SkipMOTDOperation::description() const
{
    return QStringLiteral("Skip MOTD @%1").arg(QString(serialPort()->portName()));
}

QByteArray SkipMOTDOperation::endOfMessageToken() const
{
    return QByteArrayLiteral("\r\n\r\n>: ");
}

uint32_t SkipMOTDOperation::flags() const
{
    return DTR;
}

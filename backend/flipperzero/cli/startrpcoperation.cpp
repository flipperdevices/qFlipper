#include "startrpcoperation.h"

#include <QSerialPort>

using namespace Flipper;
using namespace Zero;

StartRPCOperation::StartRPCOperation(QSerialPort *serialPort, QObject *parent):
    SimpleSerialOperation(serialPort, parent)
{}

const QString StartRPCOperation::description() const
{
    return QStringLiteral("Start RPC session @%1").arg(QString(serialPort()->portName()));
}

QByteArray StartRPCOperation::endOfMessageToken() const
{
    return commandLine() + QByteArrayLiteral("\n");
}

QByteArray StartRPCOperation::commandLine() const
{
    return QByteArrayLiteral("start_rpc_session\r");
}

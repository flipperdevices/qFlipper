#include "stoprpcoperation.h"

#include <QSerialPort>

using namespace Flipper;
using namespace Zero;

StopRPCOperation::StopRPCOperation(QSerialPort *serialPort, QObject *parent):
    AbstractSerialOperation(serialPort, parent)
{}

const QString StopRPCOperation::description() const
{
    return QStringLiteral("Stop RPC session @%1").arg(QString(serialPort()->portName()));
}

void StopRPCOperation::onSerialPortReadyRead()
{
    m_receivedData.append(serialPort()->readAll());
    if(m_receivedData.endsWith(QByteArrayLiteral("\r\n>: "))) {
        finish();
    }
}

bool StopRPCOperation::begin()
{
    return false;
}

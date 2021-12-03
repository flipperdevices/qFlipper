#include "startrpcoperation.h"

#include <QTimer>
#include <QSerialPort>

using namespace Flipper;
using namespace Zero;

StartRPCOperation::StartRPCOperation(QSerialPort *serialPort, QObject *parent):
    AbstractSerialOperation(serialPort, parent)
{}

const QString StartRPCOperation::description() const
{
    return QStringLiteral("Start RPC session @%1").arg(QString(serialPort()->portName()));
}

void StartRPCOperation::onSerialPortReadyRead()
{
    // This operation does not need serial output, discarding it
    serialPort()->clear();
}

bool StartRPCOperation::begin()
{
    const auto success = (serialPort()->write("start_rpc_session\r") > 0) && serialPort()->flush();

    if(success) {
        QTimer::singleShot(0, this, &AbstractOperation::finish);
    }

    return success;
}

#include "startrpcoperation.h"

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
    serialPort()->startTransaction();

    if(!serialPort()->readAll().endsWith(s_cmd + '\n')) {
        serialPort()->rollbackTransaction();
    } else {
        serialPort()->commitTransaction();
        finish();
    }
}

void StartRPCOperation::onOperationTimeout()
{
    if(operationState() == State::LeavingCli) {
        finishWithError(BackendError::SerialError, QStringLiteral("Failed to start RPC session"));
    } else if(operationState() == State::WaitingForPing) {
        finishWithError(BackendError::ProtocolError, QStringLiteral("No ping response from device"));
    }
}

bool StartRPCOperation::begin()
{
    setOperationState(State::LeavingCli);
    return (serialPort()->write(s_cmd) == s_cmd.size()) && serialPort()->flush();
}

const QByteArray StartRPCOperation::s_cmd("start_rpc_session\r");

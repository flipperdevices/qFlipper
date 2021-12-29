#include "startrpcoperation.h"

#include <QSerialPort>

#include "flipperzero/protobuf/systemprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

StartRPCOperation::StartRPCOperation(QSerialPort *serialPort, QObject *parent):
    AbstractProtobufOperation(serialPort, parent)
{}

const QString StartRPCOperation::description() const
{
    return QStringLiteral("Start RPC session @%1").arg(QString(serialPort()->portName()));
}

void StartRPCOperation::onSerialPortReadyRead()
{
    if(operationState() == State::LeavingCli) {
        if(serialPort()->bytesAvailable() < s_cmd.length() + 1) {
            return;
        }

        const auto str = serialPort()->readAll();

        if(!str.startsWith(s_cmd)) {
            finishWithError(QStringLiteral("Failed to send the command due to interference"));
            return;
        }

        SystemPingRequest request(serialPort());

        if(!request.send()) {
            finishWithError(QStringLiteral("Failed to send the ping request"));
        } else {
            setOperationState(State::WaitingForPing);
        }


    } else if(operationState() == State::WaitingForPing) {
        SystemPingResponse response(serialPort());

        if(!response.receive()) {
            return;
        } else if(!response.isOk()) {
            finishWithError(QStringLiteral("Device replied with error: %1").arg(response.commandStatusString()));
        } else if(!response.isValidType()) {
            finishWithError(QStringLiteral("Expected system ping response, got something else"));
        } else {
            finish();
        }

    } else {
        finishWithError(QStringLiteral("Received data in an unexpected state"));
    }
}

void StartRPCOperation::onOperationTimeout()
{
    if(operationState() == State::LeavingCli) {
        finishWithError(QStringLiteral("Failed to start RPC session"));
    } else if(operationState() == State::WaitingForPing) {
        finishWithError(QStringLiteral("No ping response from device"));
    }
}

bool StartRPCOperation::begin()
{
    setOperationState(State::LeavingCli);
    return (serialPort()->write(s_cmd) == s_cmd.size()) && serialPort()->flush();
}

const QByteArray StartRPCOperation::s_cmd("start_rpc_session\r");

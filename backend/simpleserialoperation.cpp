#include "simpleserialoperation.h"

#include <QSerialPort>

SimpleSerialOperation::SimpleSerialOperation(QSerialPort *serialPort, QObject *parent):
    AbstractSerialOperation(serialPort, parent)
{}

const QByteArray &SimpleSerialOperation::receivedData() const
{
    return m_receivedData;
}

QByteArray SimpleSerialOperation::commandLine() const
{
    // Empty default implementation
    return QByteArray();
}

uint32_t SimpleSerialOperation::flags() const
{
    // Empty default implementation
    return 0;
}

void SimpleSerialOperation::onSerialPortReadyRead()
{
    startTimeout();

    m_receivedData += serialPort()->readAll();

    if(m_receivedData.endsWith(endOfMessageToken())) {
        if(!parseReceivedData()) {
            finishWithError(BackendError::SerialError, QStringLiteral("Failed to parse received data"));
        } else {
            finish();
        }
    }
}

void SimpleSerialOperation::onOperationTimeout()
{
    finishWithError(BackendError::SerialError, QStringLiteral("Device is not responding"));
}

bool SimpleSerialOperation::begin()
{
    auto success = true;

    if(flags() & DTR) {
        success &= serialPort()->setDataTerminalReady(true);
    }

    if(flags() & RTS) {
        success &= serialPort()->setRequestToSend(flags() & RTS);
    }

    if(!commandLine().isEmpty()) {
        success &= (serialPort()->write(commandLine()) == commandLine().size()) && serialPort()->flush();
    }

    if(success) {
        startTimeout();
    }

    return success;
}

bool SimpleSerialOperation::parseReceivedData()
{
    // Empty default implementation
    return true;
}

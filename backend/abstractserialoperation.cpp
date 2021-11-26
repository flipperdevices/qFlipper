#include "abstractserialoperation.h"

#include <QTimer>
#include <QSerialPort>

#include "debug.h"

#define CALL_LATER(obj, func) (QTimer::singleShot(0, obj, func))

AbstractSerialOperation::AbstractSerialOperation(QSerialPort *serialPort, QObject *parent):
    AbstractOperation(parent),
    m_serialPort(serialPort)
{}

void AbstractSerialOperation::start()
{
    connect(m_serialPort, &QSerialPort::readyRead, this, &AbstractSerialOperation::onSerialPortReadyRead);
    connect(m_serialPort, &QSerialPort::errorOccurred, this, &AbstractSerialOperation::onSerialPortError);

    CALL_LATER(this, &AbstractSerialOperation::begin);
}

void AbstractSerialOperation::finish()
{
    disconnect(m_serialPort, &QSerialPort::readyRead, this, &AbstractSerialOperation::onSerialPortReadyRead);
    disconnect(m_serialPort, &QSerialPort::errorOccurred, this, &AbstractSerialOperation::onSerialPortError);

    AbstractOperation::finish();
}

QSerialPort *AbstractSerialOperation::serialPort() const
{
    return m_serialPort;
}

void AbstractSerialOperation::onSerialPortError()
{
    finishWithError(m_serialPort->errorString());
}

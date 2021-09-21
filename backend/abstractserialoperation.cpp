#include "abstractserialoperation.h"

#include <QTimer>
#include <QSerialPort>

#include "macros.h"

AbstractSerialOperation::AbstractSerialOperation(QSerialPort *serialPort, QObject *parent):
    AbstractOperation(parent),
    m_serialPort(serialPort)
{}

AbstractSerialOperation::~AbstractSerialOperation()
{}

void AbstractSerialOperation::start()
{
    connect(m_serialPort, &QSerialPort::readyRead, this, &AbstractSerialOperation::onSerialPortReadyRead);
    connect(m_serialPort, &QSerialPort::errorOccurred, this, &AbstractSerialOperation::onSerialPortError);

    QTimer::singleShot(0, this, &AbstractSerialOperation::begin);
}

void AbstractSerialOperation::finish()
{
    stopTimeout();
    setState(BasicState::Finished);

    disconnect(m_serialPort, &QSerialPort::readyRead, this, &AbstractSerialOperation::onSerialPortReadyRead);
    disconnect(m_serialPort, &QSerialPort::errorOccurred, this, &AbstractSerialOperation::onSerialPortError);

    emit finished();
}

QSerialPort *AbstractSerialOperation::serialPort() const
{
    return m_serialPort;
}

void AbstractSerialOperation::onSerialPortError()
{
    finishWithError(m_serialPort->errorString());
}

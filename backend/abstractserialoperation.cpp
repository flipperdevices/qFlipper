#include "abstractserialoperation.h"

#include <QTimer>
#include <QSerialPort>

AbstractSerialOperation::AbstractSerialOperation(QSerialPort *serialPort, QObject *parent):
    AbstractOperation(parent),
    m_serialPort(serialPort),
    m_totalBytesWritten(0)
{}

void AbstractSerialOperation::start()
{
    connect(m_serialPort, &QSerialPort::readyRead, this, &AbstractSerialOperation::startTimeout);
    connect(m_serialPort, &QSerialPort::bytesWritten, this, &AbstractSerialOperation::startTimeout);
    connect(m_serialPort, &QSerialPort::readyRead, this, &AbstractSerialOperation::onSerialPortReadyRead);
    connect(m_serialPort, &QSerialPort::errorOccurred, this, &AbstractSerialOperation::onSerialPortError);
    connect(m_serialPort, &QSerialPort::bytesWritten, this, &AbstractSerialOperation::onSerialPortBytesWritten);

    QTimer::singleShot(0, this, [=]() {
        if(!begin()) {
            finishWithError(BackendError::SerialError, QStringLiteral("Failed to begin operation: %1").arg(m_serialPort->errorString()));
        } else {
            startTimeout();
        }
    });
}

void AbstractSerialOperation::finish()
{
    disconnect(m_serialPort, &QSerialPort::readyRead, this, &AbstractSerialOperation::startTimeout);
    disconnect(m_serialPort, &QSerialPort::bytesWritten, this, &AbstractSerialOperation::startTimeout);
    disconnect(m_serialPort, &QSerialPort::readyRead, this, &AbstractSerialOperation::onSerialPortReadyRead);
    disconnect(m_serialPort, &QSerialPort::errorOccurred, this, &AbstractSerialOperation::onSerialPortError);
    disconnect(m_serialPort, &QSerialPort::bytesWritten, this, &AbstractSerialOperation::onSerialPortBytesWritten);

    AbstractOperation::finish();
}

QSerialPort *AbstractSerialOperation::serialPort() const
{
    return m_serialPort;
}

qint64 AbstractSerialOperation::totalBytesWritten() const
{
    return m_totalBytesWritten;
}

void AbstractSerialOperation::resetTotalBytesWritten()
{
    m_totalBytesWritten = 0;
}

void AbstractSerialOperation::onSerialPortReadyRead()
{
    // Empty default implementation
}

void AbstractSerialOperation::onTotalBytesWrittenChanged()
{
    // Empty default implementation
}

void AbstractSerialOperation::onSerialPortBytesWritten(qint64 numBytes)
{
    m_totalBytesWritten += numBytes;
    onTotalBytesWrittenChanged();
}

void AbstractSerialOperation::onSerialPortError()
{
    finishWithError(BackendError::SerialError, m_serialPort->errorString());
}

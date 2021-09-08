#include "storageoperation.h"

#include <QSerialPort>

#include "macros.h"

using namespace Flipper;
using namespace Zero;

StorageOperation::StorageOperation(QSerialPort *serialPort, QObject *parent):
    AbstractOperation(parent),
    m_serialPort(serialPort)
{}

StorageOperation::~StorageOperation()
{}

void StorageOperation::start()
{
    connect(m_serialPort, &QSerialPort::readyRead, this, &StorageOperation::onSerialPortReadyRead);
    connect(m_serialPort, &QSerialPort::errorOccurred, this, &StorageOperation::onSerialPortError);

    begin();
}

void StorageOperation::finish()
{
    disconnect(m_serialPort, &QSerialPort::readyRead, this, &StorageOperation::onSerialPortReadyRead);
    disconnect(m_serialPort, &QSerialPort::errorOccurred, this, &StorageOperation::onSerialPortError);
}

QSerialPort *StorageOperation::serialPort() const
{
    return m_serialPort;
}

void StorageOperation::onSerialPortError()
{
    qDebug() << "Serial port error:" << m_serialPort->errorString();
}

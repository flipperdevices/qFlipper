#include "storagecontroller.h"

#include <QTimer>
#include <QSerialPort>

#include "storage/statoperation.h"

#include "macros.h"

using namespace Flipper;
using namespace Zero;


StorageController::StorageController(const QSerialPortInfo &portInfo, QObject *parent):
    QObject(parent),
    m_serialPort(new QSerialPort(portInfo, this)),
    m_responseTimer(new QTimer(this)),
    m_state(State::Idle)
{
    connect(m_serialPort, &QSerialPort::readyRead, this, &StorageController::onSerialreadyRead);
    connect(m_serialPort, &QSerialPort::errorOccurred, this, &StorageController::onSerialErrorOccured);
    connect(m_responseTimer, &QTimer::timeout, this, &StorageController::processQueue);

    m_responseTimer->setSingleShot(true);
}

StorageController::~StorageController()
{}

StatOperation *StorageController::stat(const QByteArray &fileName)
{
    auto *op = new StatOperation(m_serialPort, fileName, this);
    enqueueOperation(op);
    return op;
}

void StorageController::processQueue()
{
    if(m_operationQueue.isEmpty()) {
        m_state = State::Idle;
        closePort();
        return;
    }

    if(m_state == State::Idle) {
        if(!openPort()) {
            // TODO: error signaling
            m_state = State::ErrorOccured;
            qDebug() << "Serial port error:" << m_serialPort->errorString();
        } else {
            m_responseTimer->start(1000);
            m_state = State::SkippingMOTD;
        }

    } else if(m_state == State::SkippingMOTD) {
        const auto leftover = m_serialPort->readAll().trimmed();

        if(leftover != QByteArrayLiteral(">:")) {
            error_msg("Failed to detect the prompt.");
            return;
        }

        m_state = State::ExecutingOperation;
        processQueue();

    } else if(m_state == State::ExecutingOperation) {
        auto *op = m_operationQueue.dequeue();

        connect(op, &AbstractOperation::finished, this, [=]() {
            if(op->isError()) {
                error_msg(QStringLiteral("Operation finished with error: %1").arg(op->errorString()));
            } else {
                processQueue();
            }
        });

        op->start();
    }
}

void StorageController::onSerialreadyRead()
{
    if(m_state != State::SkippingMOTD) {
        return;
    }

    m_responseTimer->stop();

    while(m_serialPort->canReadLine()) {
        m_serialPort->readLine();
    }

    m_responseTimer->start(50);
}

void StorageController::onSerialErrorOccured()
{

}

//void StorageController::onRequestTimeout()
//{

//}

bool StorageController::openPort()
{
    const auto success = m_serialPort->open(QIODevice::ReadWrite) &&
                         m_serialPort->setDataTerminalReady(true);
    if(!success) {
        error_msg(QStringLiteral("Failed to open serial port: %1").arg(m_serialPort->errorString()));
    }

    return success;
}

void StorageController::closePort()
{
    m_serialPort->flush();
    m_serialPort->clear();
    m_serialPort->close();
}

void StorageController::enqueueOperation(StorageOperation *op)
{
    m_operationQueue.enqueue(op);

    if(m_state != State::Idle) {
       return;
    }

    processQueue();
}

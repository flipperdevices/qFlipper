#include "storagecontroller.h"

#include <QTimer>
#include <QSerialPort>

#include "common/skipmotdoperation.h"
#include "storage/removeoperation.h"
#include "storage/mkdiroperation.h"
#include "storage/writeoperation.h"
#include "storage/readoperation.h"
#include "storage/statoperation.h"

#include "macros.h"

using namespace Flipper;
using namespace Zero;

StorageController::StorageController(const QSerialPortInfo &portInfo, QObject *parent):
    QObject(parent),
    m_serialPort(new QSerialPort(portInfo, this)),
    m_state(State::Idle)
{}

StorageController::~StorageController()
{}

StatOperation *StorageController::stat(const QByteArray &fileName)
{
    auto *op = new StatOperation(m_serialPort, fileName, this);
    enqueueOperation(op);
    return op;
}

ReadOperation *StorageController::read(const QByteArray &fileName)
{
    auto *op = new ReadOperation(m_serialPort, fileName, this);
    enqueueOperation(op);
    return op;
}

MkDirOperation *StorageController::mkdir(const QByteArray &dirName)
{
    auto *op = new MkDirOperation(m_serialPort, dirName, this);
    enqueueOperation(op);
    return op;
}

WriteOperation *StorageController::write(const QByteArray &fileName, QIODevice *file)
{
    auto *op = new WriteOperation(m_serialPort, fileName, file, this);
    enqueueOperation(op);
    return op;
}

RemoveOperation *StorageController::remove(const QByteArray &fileName)
{
    auto *op = new RemoveOperation(m_serialPort, fileName, this);
    enqueueOperation(op);
    return op;
}

void StorageController::processQueue()
{
    if(m_operationQueue.isEmpty()) {
        m_state = State::Idle;
        m_serialPort->close();
        return;
    }

    auto *currentOperation = m_operationQueue.dequeue();

    connect(currentOperation, &AbstractOperation::finished, this, [=]() {

        if(currentOperation->isError()) {
            qDebug() << "Operation error:" << currentOperation->errorString();
        } else {
            processQueue();
        }

        currentOperation->deleteLater();
    });

    currentOperation->start();
}

void StorageController::enqueueOperation(AbstractSerialOperation *op)
{
    m_operationQueue.enqueue(op);

    if(m_state == State::Idle) {

        if(!m_serialPort->open(QIODevice::ReadWrite)) {
            qDebug() << "Serial port error:" << m_serialPort->errorString();
            return;
        }

        m_state = State::Running;
        m_operationQueue.prepend(new SkipMOTDOperation(m_serialPort, this));

        QTimer::singleShot(0, this, &StorageController::processQueue);
    }
}

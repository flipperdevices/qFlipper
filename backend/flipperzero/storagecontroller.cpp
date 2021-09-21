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
    SignalingFailable(parent),
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

ReadOperation *StorageController::read(const QByteArray &fileName, QIODevice *file)
{
    auto *op = new ReadOperation(m_serialPort, fileName, file, this);
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
        closePort();
        return;
    }

    auto *currentOperation = m_operationQueue.dequeue();

    connect(currentOperation, &AbstractOperation::finished, this, [=]() {

        if(currentOperation->isError()) {
            clearQueue();
            processQueue();
            setError(QStringLiteral("Operation error: %1").arg(currentOperation->errorString()));

        } else {
            processQueue();
        }

        currentOperation->deleteLater();
    });

    currentOperation->start();
}

bool StorageController::openPort()
{
    if(!m_serialPort->open(QIODevice::ReadWrite)) {
        setError(QStringLiteral("Serial port error: %1").arg(m_serialPort->errorString()));
        return false;
    }

    m_state = State::Running;
    m_operationQueue.prepend(new SkipMOTDOperation(m_serialPort, this));

    return true;
}

void StorageController::closePort()
{
    m_state = State::Idle;
    m_serialPort->close();
}

void StorageController::enqueueOperation(AbstractSerialOperation *op)
{
    m_operationQueue.enqueue(op);

    if(m_state == State::Idle) {
        if(!openPort()) {
            return;
        }

        resetError();
        QTimer::singleShot(0, this, &StorageController::processQueue);
    }
}

void StorageController::clearQueue()
{
    while(!m_operationQueue.isEmpty()) {
        m_operationQueue.dequeue()->deleteLater();
    }
}

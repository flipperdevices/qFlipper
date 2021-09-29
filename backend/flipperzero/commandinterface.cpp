#include "commandinterface.h"

#include <QTimer>
#include <QSerialPort>

#include "cli/skipmotdoperation.h"
#include "cli/removeoperation.h"
#include "cli/mkdiroperation.h"
#include "cli/writeoperation.h"
#include "cli/readoperation.h"
#include "cli/statoperation.h"
#include "cli/listoperation.h"

#include "macros.h"

using namespace Flipper;
using namespace Zero;

CommandInterface::CommandInterface(const QSerialPortInfo &portInfo, QObject *parent):
    SignalingFailable(parent),
    m_serialPort(new QSerialPort(portInfo, this)),
    m_state(State::Idle)
{}

CommandInterface::~CommandInterface()
{}

ListOperation *CommandInterface::list(const QByteArray &dirName)
{
    auto *op = new ListOperation(m_serialPort, dirName, this);
    enqueueOperation(op);
    return op;

}

StatOperation *CommandInterface::stat(const QByteArray &fileName)
{
    auto *op = new StatOperation(m_serialPort, fileName, this);
    enqueueOperation(op);
    return op;
}

ReadOperation *CommandInterface::read(const QByteArray &fileName, QIODevice *file)
{
    auto *op = new ReadOperation(m_serialPort, fileName, file, this);
    enqueueOperation(op);
    return op;
}

MkDirOperation *CommandInterface::mkdir(const QByteArray &dirName)
{
    auto *op = new MkDirOperation(m_serialPort, dirName, this);
    enqueueOperation(op);
    return op;
}

WriteOperation *CommandInterface::write(const QByteArray &fileName, QIODevice *file)
{
    auto *op = new WriteOperation(m_serialPort, fileName, file, this);
    enqueueOperation(op);
    return op;
}

RemoveOperation *CommandInterface::remove(const QByteArray &fileName)
{
    auto *op = new RemoveOperation(m_serialPort, fileName, this);
    enqueueOperation(op);
    return op;
}

void CommandInterface::processQueue()
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

bool CommandInterface::openPort()
{
    if(!m_serialPort->open(QIODevice::ReadWrite)) {
        setError(QStringLiteral("Serial port error: %1").arg(m_serialPort->errorString()));
        return false;
    }

    m_state = State::Running;
    m_operationQueue.prepend(new SkipMOTDOperation(m_serialPort, this));

    return true;
}

void CommandInterface::closePort()
{
    m_state = State::Idle;
    m_serialPort->close();
}

void CommandInterface::enqueueOperation(AbstractSerialOperation *op)
{
    m_operationQueue.enqueue(op);

    if(m_state == State::Idle) {
        if(!openPort()) {
            return;
        }

        resetError();
        QTimer::singleShot(0, this, &CommandInterface::processQueue);
    }
}

void CommandInterface::clearQueue()
{
    while(!m_operationQueue.isEmpty()) {
        m_operationQueue.dequeue()->deleteLater();
    }
}

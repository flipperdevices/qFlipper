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
    AbstractOperationRunner(parent),
    m_serialPort(new QSerialPort(portInfo, this))
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

bool CommandInterface::onQueueStarted()
{
    const auto success = m_serialPort->open(QIODevice::ReadWrite);
    check_return_bool(success, QStringLiteral("Serial port error: %1").arg(m_serialPort->errorString()));

    enqueueOperation(new SkipMOTDOperation(m_serialPort, this));
    return true;
}

bool CommandInterface::onQueueFinished()
{
    m_serialPort->close();
    return true;
}

#include "commandinterface.h"

#include <QSerialPort>
#include <QLoggingCategory>

#include "flipperzero/devicestate.h"

#include "cli/systemfactoryresetoperation.h"
#include "cli/skipmotdoperation.h"
#include "cli/startrpcoperation.h"
#include "cli/stoprpcoperation.h"
#include "cli/systemrebootoperation.h"
#include "cli/storageremoveoperation.h"
#include "cli/storagemkdiroperation.h"
#include "cli/storagewriteoperation.h"
#include "cli/storagereadoperation.h"
#include "cli/storagelistoperation.h"
#include "cli/storagestatoperation.h"
#include "cli/storageinfooperation.h"

Q_LOGGING_CATEGORY(CATEGORY_RPC, "RPC");

using namespace Flipper;
using namespace Zero;

CommandInterface::CommandInterface(DeviceState *state, QObject *parent):
    AbstractOperationRunner(parent),
    m_deviceState(state)
{}

StopRPCOperation *CommandInterface::stopRPCSession()
{
    return registerOperation(new StopRPCOperation(serialPort(), this));
}

StartRPCOperation *CommandInterface::startRPCSession()
{
    return registerOperation(new StartRPCOperation(serialPort(), this));
}

SystemRebootOperation *CommandInterface::rebootToOS()
{
    return registerOperation(new SystemRebootOperation(serialPort(), SystemRebootOperation::RebootType::OS, this));
}

SystemRebootOperation *CommandInterface::rebootToRecovery()
{
    return registerOperation(new SystemRebootOperation(serialPort(), SystemRebootOperation::RebootType::Recovery, this));
}

SystemFactoryResetOperation *CommandInterface::factoryReset()
{
    return registerOperation(new SystemFactoryResetOperation(serialPort(), this));
}

StorageListOperation *CommandInterface::storageList(const QByteArray &path)
{
    return registerOperation(new StorageListOperation(serialPort(), path, this));
}

StorageInfoOperation *CommandInterface::storageInfo(const QByteArray &path)
{
    return registerOperation(new StorageInfoOperation(serialPort(), path, this));
}

StorageStatOperation *CommandInterface::storageStat(const QByteArray &path)
{
    return registerOperation(new StorageStatOperation(serialPort(), path, this));
}

StorageReadOperation *CommandInterface::storageRead(const QByteArray &path, QIODevice *file)
{
    return registerOperation(new StorageReadOperation(serialPort(), path, file, this));
}

StorageMkdirOperation *CommandInterface::storageMkdir(const QByteArray &path)
{
    return registerOperation(new StorageMkdirOperation(serialPort(), path, this));
}

StorageWriteOperation *CommandInterface::storageWrite(const QByteArray &path, QIODevice *file)
{
    return registerOperation(new StorageWriteOperation(serialPort(), path, file, this));
}

StorageRemoveOperation *CommandInterface::storageRemove(const QByteArray &path)
{
    return registerOperation(new StorageRemoveOperation(serialPort(), path, this));
}

const QLoggingCategory &CommandInterface::loggingCategory() const
{
    return CATEGORY_RPC();
}

QSerialPort *CommandInterface::serialPort() const
{
    return m_deviceState->deviceInfo().serialPort.get();
}

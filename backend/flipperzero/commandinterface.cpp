#include "commandinterface.h"

#include <QLoggingCategory>

#include "flipperzero/devicestate.h"

#include "rpc/skipmotdoperation.h"

#include "rpc/startrpcoperation.h"
#include "rpc/stoprpcoperation.h"

#include "rpc/systemrebootoperation.h"
#include "rpc/systemfactoryresetoperation.h"

#include "rpc/storageremoveoperation.h"
#include "rpc/storagemkdiroperation.h"
#include "rpc/storagewriteoperation.h"
#include "rpc/storagereadoperation.h"
#include "rpc/storagelistoperation.h"
#include "rpc/storagestatoperation.h"
#include "rpc/storageinfooperation.h"

#include "rpc/guistartstreamoperation.h"
#include "rpc/guistopstreamoperation.h"

Q_LOGGING_CATEGORY(CATEGORY_RPC, "RPC");

using namespace Flipper;
using namespace Zero;

CommandInterface::CommandInterface(DeviceState *deviceState, QObject *parent):
    AbstractOperationRunner(parent),
    m_deviceState(deviceState)
{}

QSerialPort *CommandInterface::serialPort() const
{
    return m_deviceState->serialPort();
}

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

GuiStartStreamOperation *CommandInterface::guiStartStreaming()
{
    return registerOperation(new GuiStartStreamOperation(serialPort(), this));
}

GuiStopStreamOperation *CommandInterface::guiStopStreaming()
{
    return registerOperation(new GuiStopStreamOperation(serialPort(), this));
}

StorageRemoveOperation *CommandInterface::storageRemove(const QByteArray &path)
{
    return registerOperation(new StorageRemoveOperation(serialPort(), path, this));
}

const QLoggingCategory &CommandInterface::loggingCategory() const
{
    return CATEGORY_RPC();
}

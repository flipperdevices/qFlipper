#include "commandinterface.h"

#include <QLoggingCategory>

#include "flipperzero/devicestate.h"

#include "rpc/startrpcoperation.h"
#include "rpc/stoprpcoperation.h"

#include "rpc/systemrebootoperation.h"
#include "rpc/systemfactoryresetoperation.h"

#include "rpc/storageremoveoperation.h"
#include "rpc/storagemkdiroperation.h"
#include "rpc/storagewriteoperation.h"
#include "rpc/storagereadoperation.h"
#include "rpc/storagelistoperation.h"

Q_LOGGING_CATEGORY(CATEGORY_RPC, "RPC");

using namespace Flipper;
using namespace Zero;

CommandInterface::CommandInterface(DeviceState *deviceState, QObject *parent):
    AbstractOperationRunner(parent),
    m_deviceState(deviceState)
{}

QSerialPort *CommandInterface::serialPort() const
{
    return nullptr;
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
    Q_UNUSED(path)
    return nullptr;
}

StorageInfoOperation *CommandInterface::storageInfo(const QByteArray &path)
{
    Q_UNUSED(path)
    return nullptr;
}

StorageStatOperation *CommandInterface::storageStat(const QByteArray &path)
{
    Q_UNUSED(path)
    return nullptr;
}

StorageReadOperation *CommandInterface::storageRead(const QByteArray &path, QIODevice *file)
{
    return registerOperation(new StorageReadOperation(serialPort(), path, file, this));
}

StorageMkdirOperation *CommandInterface::storageMkdir(const QByteArray &path)
{
    Q_UNUSED(path)
    return nullptr;
}

StorageWriteOperation *CommandInterface::storageWrite(const QByteArray &path, QIODevice *file)
{
    Q_UNUSED(path)
    Q_UNUSED(file)
    return nullptr;
}

GuiStartScreenStreamOperation *CommandInterface::guiStartStreaming()
{
    return nullptr;
}

GuiStopScreenStreamOperation *CommandInterface::guiStopStreaming()
{
    return nullptr;
}

GuiStartVirtualDisplayOperation *CommandInterface::guiStartVirtualDisplay(const QByteArray &screenData)
{
    Q_UNUSED(screenData)
    return nullptr;
}

GuiStopVirtualDisplayOperation *CommandInterface::guiStopVirtualDisplay()
{
    return nullptr;
}

GuiScreenFrameOperation *CommandInterface::guiSendScreenFrame(const QByteArray &screenData)
{
    Q_UNUSED(screenData)
    return nullptr;
}

StorageRemoveOperation *CommandInterface::storageRemove(const QByteArray &path)
{
    Q_UNUSED(path)
    return nullptr;
}

const QLoggingCategory &CommandInterface::loggingCategory() const
{
    return CATEGORY_RPC();
}

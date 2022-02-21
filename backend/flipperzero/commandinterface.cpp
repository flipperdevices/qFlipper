#include "commandinterface.h"

#include <QLoggingCategory>

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
    return nullptr;
}

SystemRebootOperation *CommandInterface::rebootToRecovery()
{
    return nullptr;
}

SystemFactoryResetOperation *CommandInterface::factoryReset()
{
    return nullptr;
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
    Q_UNUSED(path)
    Q_UNUSED(file)
    return nullptr;
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

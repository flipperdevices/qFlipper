#pragma once

#include "abstractoperationrunner.h"

class QIODevice;
class QSerialPort;

namespace Flipper {
namespace Zero {

class DeviceState;

class StopRPCOperation;
class StartRPCOperation;

class SystemRebootOperation;
class SystemFactoryResetOperation;

class StorageListOperation;
class StorageInfoOperation;
class StorageStatOperation;
class StorageReadOperation;
class StorageMkdirOperation;
class StorageWriteOperation;
class StorageRemoveOperation;

class GuiStartScreenStreamOperation;
class GuiStopScreenStreamOperation;
class GuiScreenFrameOperation;
class GuiStartVirtualDisplayOperation;
class GuiStopVirtualDisplayOperation;

class CommandInterface : public AbstractOperationRunner
{
    Q_OBJECT

public:
    CommandInterface(DeviceState *deviceState, QObject *parent = nullptr);
    QSerialPort *serialPort() const;

    SystemRebootOperation *rebootToOS();
    SystemRebootOperation *rebootToRecovery();
    SystemFactoryResetOperation *factoryReset();

    StorageListOperation *storageList(const QByteArray &path);
    StorageInfoOperation *storageInfo(const QByteArray &path);
    StorageStatOperation *storageStat(const QByteArray &path);
    StorageMkdirOperation *storageMkdir(const QByteArray &path);
    StorageRemoveOperation *storageRemove(const QByteArray &path);
    StorageReadOperation *storageRead(const QByteArray &path, QIODevice *file);
    StorageWriteOperation *storageWrite(const QByteArray &path, QIODevice *file);

    GuiStartScreenStreamOperation *guiStartStreaming();
    GuiStopScreenStreamOperation *guiStopStreaming();

    GuiStartVirtualDisplayOperation *guiStartVirtualDisplay(const QByteArray &screenData = QByteArray());
    GuiStopVirtualDisplayOperation *guiStopVirtualDisplay();
    GuiScreenFrameOperation *guiSendScreenFrame(const QByteArray &screenData);

private:
    const QLoggingCategory &loggingCategory() const override;
    DeviceState *m_deviceState;
};

}
}

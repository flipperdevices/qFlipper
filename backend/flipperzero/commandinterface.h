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

class GuiStartStreamOperation;
class GuiStopStreamOperation;

class CommandInterface : public AbstractOperationRunner
{
    Q_OBJECT

public:
    CommandInterface(DeviceState *state, QObject *parent = nullptr);

    StopRPCOperation *stopRPCSession();
    StartRPCOperation *startRPCSession();

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

    GuiStartStreamOperation *guiStartStreaming();
    GuiStopStreamOperation *guiStopStreaming();

private:
    const QLoggingCategory &loggingCategory() const override;
    QSerialPort *serialPort() const;
    DeviceState *m_deviceState;
};

}
}

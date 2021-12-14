#pragma once

#include "abstractoperationrunner.h"

class QIODevice;
class QSerialPort;
class QSerialPortInfo;

namespace Flipper {
namespace Zero {

class DeviceState;

class DFUOperation;
class RebootOperation;
class StopRPCOperation;
class StartRPCOperation;
class SystemFactoryResetOperation;

class StorageListOperation;
class StorageInfoOperation;
class StorageStatOperation;
class StorageReadOperation;
class StorageMkdirOperation;
class StorageWriteOperation;
class StorageRemoveOperation;

class CommandInterface : public AbstractOperationRunner
{
    Q_OBJECT

public:
    CommandInterface(DeviceState *state, QObject *parent = nullptr);

    RebootOperation *reboot();
    DFUOperation *startRecoveryMode();
    StopRPCOperation *stopRPCSession();
    StartRPCOperation *startRPCSession();
    SystemFactoryResetOperation *factoryReset();

    StorageListOperation *storageList(const QByteArray &path);
    StorageInfoOperation *storageInfo(const QByteArray &path);
    StorageStatOperation *storageStat(const QByteArray &path);
    StorageReadOperation *storageRead(const QByteArray &path, QIODevice *file);
    StorageMkdirOperation *storageMkdir(const QByteArray &path);
    StorageWriteOperation *storageWrite(const QByteArray &path, QIODevice *file);
    StorageRemoveOperation *storageRemove(const QByteArray &path);

private:
    bool onQueueStarted() override;
    bool onQueueFinished() override;

    const QLoggingCategory &loggingCategory() const override;

    QSerialPort *m_serialPort;
};

}
}

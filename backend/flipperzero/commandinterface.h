#pragma once

#include "abstractoperationrunner.h"

class QIODevice;
class QSerialPort;
class QSerialPortInfo;

namespace Flipper {
namespace Zero {

class DeviceState;

class DFUOperation;
class StorageListOperation;
class StorageInfoOperation;
class StorageStatOperation;
class ReadOperation;
class MkDirOperation;
class StorageWriteOperation;
class RemoveOperation;
class RebootOperation;
class StopRPCOperation;
class StartRPCOperation;
class FactoryResetCliOperation;

class CommandInterface : public AbstractOperationRunner
{
    Q_OBJECT

public:
    CommandInterface(DeviceState *state, QObject *parent = nullptr);

    RebootOperation *reboot();
    DFUOperation *startRecoveryMode();
    StopRPCOperation *stopRPCSession();
    StartRPCOperation *startRPCSession();
    FactoryResetCliOperation *factoryReset();

    StorageListOperation *storageList(const QByteArray &path);
    StorageInfoOperation *storageInfo(const QByteArray &path);
    StorageStatOperation *storageStat(const QByteArray &path);
    ReadOperation *read(const QByteArray &fileName, QIODevice *file);
    MkDirOperation *mkdir(const QByteArray &dirName);
    StorageWriteOperation *storageWrite(const QByteArray &fileName, QIODevice *file);
    RemoveOperation *remove(const QByteArray &fileName);

private:
    bool onQueueStarted() override;
    bool onQueueFinished() override;

    const QLoggingCategory &loggingCategory() const override;

    QSerialPort *m_serialPort;
};

}
}

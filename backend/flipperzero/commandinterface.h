#pragma once

#include "abstractoperationrunner.h"

class QIODevice;
class QSerialPort;
class QSerialPortInfo;

namespace Flipper {
namespace Zero {

class DeviceState;

class DFUOperation;
class ListOperation;
class StatOperation;
class ReadOperation;
class MkDirOperation;
class WriteOperation;
class RemoveOperation;
class RebootOperation;
class StartRPCOperation;
class FactoryResetCliOperation;

class CommandInterface : public AbstractOperationRunner
{
    Q_OBJECT

public:
    CommandInterface(DeviceState *state, QObject *parent = nullptr);

    RebootOperation *reboot();
    DFUOperation *startRecoveryMode();
    StartRPCOperation *startRPCSession();
    FactoryResetCliOperation *factoryReset();

    ListOperation *list(const QByteArray &dirName);
    StatOperation *stat(const QByteArray &fileName);
    ReadOperation *read(const QByteArray &fileName, QIODevice *file);
    MkDirOperation *mkdir(const QByteArray &dirName);
    WriteOperation *write(const QByteArray &fileName, QIODevice *file);
    RemoveOperation *remove(const QByteArray &fileName);


private:
    bool onQueueStarted() override;
    bool onQueueFinished() override;

    const QLoggingCategory &loggingCategory() const override;

    QSerialPort *m_serialPort;
};

}
}

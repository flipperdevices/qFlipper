#pragma once

#include <QQueue>
#include <QSerialPortInfo>

#include "signalingfailable.h"

class QIODevice;
class QSerialPort;

class AbstractSerialOperation;

namespace Flipper {
namespace Zero {

class StatOperation;
class ReadOperation;
class MkDirOperation;
class WriteOperation;
class RemoveOperation;

class StorageController : public SignalingFailable
{
    Q_OBJECT

    using OperationQueue = QQueue<AbstractSerialOperation*>;

    enum class State {
        Idle,
        Running
    };

public:
    StorageController(const QSerialPortInfo &portInfo, QObject *parent = nullptr);
    ~StorageController();

    StatOperation *stat(const QByteArray &fileName);
    ReadOperation *read(const QByteArray &fileName, QIODevice *file);
    MkDirOperation *mkdir(const QByteArray &dirName);
    WriteOperation *write(const QByteArray &fileName, QIODevice *file);
    RemoveOperation *remove(const QByteArray &fileName);

private slots:
    void processQueue();

private:
    bool openPort();
    void closePort();

    void enqueueOperation(AbstractSerialOperation *op);
    void clearQueue();

    OperationQueue m_operationQueue;
    QSerialPort *m_serialPort;
    State m_state;
};

}
}

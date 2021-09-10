#pragma once

#include <QQueue>
#include <QObject>
#include <QSerialPortInfo>

class QSerialPort;

class AbstractSerialOperation;

namespace Flipper {
namespace Zero {

class StatOperation;
class RemoveOperation;

// BIG TODO: Error signaling
class StorageController : public QObject
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
    RemoveOperation *remove(const QByteArray &fileName);

private slots:
    void processQueue();

private:
    void enqueueOperation(AbstractSerialOperation *op);

    OperationQueue m_operationQueue;
    QSerialPort *m_serialPort;
    State m_state;
};

}
}

#pragma once

#include <QQueue>
#include <QObject>
#include <QSerialPortInfo>

class QTimer;
class QSerialPort;

namespace Flipper {
namespace Zero {

class StorageOperation;
class StatOperation;

// BIG TODO: Error signaling
class StorageController : public QObject
{
    Q_OBJECT

    using OperationQueue = QQueue<StorageOperation*>;

    enum class State {
        Idle,
        SkippingMOTD,
        ExecutingOperation,
        ErrorOccured
    };

public:
    StorageController(const QSerialPortInfo &portInfo, QObject *parent = nullptr);
    ~StorageController();

    StatOperation *stat(const QByteArray &fileName);

private slots:
    void processQueue();
    void onSerialreadyRead();
    void onSerialErrorOccured();

private:
    bool openPort();
    void closePort();

    void enqueueOperation(StorageOperation *op);

    OperationQueue m_operationQueue;
    QSerialPort *m_serialPort;
    QTimer *m_responseTimer;
    State m_state;
};

}
}

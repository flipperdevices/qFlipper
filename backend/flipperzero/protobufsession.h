#pragma once

#include <QQueue>
#include <QObject>
#include <QSerialPortInfo>

#include "failable.h"

class ProtobufPluginInterface;

namespace Flipper {
namespace Zero {

class AbstractProtobufOperation;
class SystemDeviceInfoOperation;

class ProtobufSession : public QObject, public Failable
{
    Q_OBJECT

public:
    enum SessionState {
        Starting,
        Idle,
        Running,
        Stopping,
        Stopped
    };

    ProtobufSession(const QSerialPortInfo &serialInfo, QObject *parent = nullptr);
    ~ProtobufSession();

    SessionState sessionState() const;

    void setSerialPort(const QSerialPortInfo &serialInfo);

    void setMajorVersion(int versionMajor);
    void setMinorVersion(int versionMinor);

    // Operations
    SystemDeviceInfoOperation *systemDeviceInfo();

signals:
    void sessionStateChanged();
    void broadcastResponseReceived(QObject *response);

public slots:
//    void start();
    void stop();

private slots:
    void onSerialPortReadyRead();
    void onSerialPortBytesWriten(qint64 nbytes);
    void onSerialPortErrorOccured();

    void processQueue();
    void writeToPort();

private:
    bool loadProtobufPlugin();
    bool unloadProtobufPlugin();

    const QString protobufPluginPath() const;
    uint32_t getAndIncrementCounter();
    void setSessionState(SessionState newState);

    template<class T>
    T* enqueueOperation(T *operation);

    void processMatchedResponse(QObject *response);
    void processBroadcastResponse(QObject *response);
    void processUnmatchedResponse(QObject *response);
    void processErrorResponse(QObject *response);

    SessionState m_sessionState;
    QSerialPort *m_serialPort;
    QByteArray m_receivedData;

    ProtobufPluginInterface *m_plugin;
    QQueue<AbstractProtobufOperation*> m_queue;
    AbstractProtobufOperation *m_currentOperation;

    qint64 m_bytesToWrite;
    uint32_t m_counter;

    int m_versionMajor;
    int m_versionMinor;
};

}
}


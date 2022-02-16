#pragma once

#include <QObject>
#include <QSerialPortInfo>

class ProtobufPluginInterface;

namespace Flipper {
namespace Zero {

class ProtobufSession : public QObject
{
    Q_OBJECT

public:
    enum SessionState {
        Starting,
        Running,
        Stopping,
        Stopped,
        ErrorOccured
    };

    ProtobufSession(const QSerialPortInfo &serialInfo, QObject *parent = nullptr);
    ~ProtobufSession();

    SessionState sessionState() const;

    void setSerialPort(const QSerialPortInfo &serialInfo);

    void setMajorVersion(int versionMajor);
    void setMinorVersion(int versionMinor);

signals:
    void sessionStateChanged();

public slots:
    void start();
    void stop();

private slots:
    void onSerialPortBytesWriten();
    void onSerialPortReadyRead();
    void onSerialPortErrorOccured();

private:
    bool loadProtobufPlugin();
    bool unloadProtobufPlugin();

    uint32_t getAndIncrementCounter();
    void setSessionState(SessionState newState);

    const QString protobufPluginPath() const;

    SessionState m_sessionState;
    QSerialPort *m_serialPort;

    ProtobufPluginInterface *m_plugin;

    uint32_t m_counter;

    int m_versionMajor;
    int m_versionMinor;
};

}
}


#pragma once

#include <QObject>
#include <QSerialPortInfo>

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
    void setSessionState(SessionState newState);

    SessionState m_sessionState;
    QSerialPort *m_serialPort;
    uint32_t m_messageID;
};

}
}


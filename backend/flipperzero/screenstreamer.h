#pragma once

#include <QObject>
#include <QByteArray>

class QSerialPort;

namespace Flipper {
namespace Zero {

class DeviceState;
class CommandInterface;

class ScreenStreamer : public QObject
{
    Q_OBJECT

public:
    enum class State {
        Starting,
        Running,
        Stopping,
        Stopped
    };

    Q_ENUM(State)

    ScreenStreamer(DeviceState *deviceState, CommandInterface *rpc, QObject *parent = nullptr);
    void sendInputEvent(int key, int type);

public slots:
    void start();
    void stop();

private slots:
    void onPortReadyRead();

private:
    void sendStopCommand();
    void setState(State newState);

    QSerialPort *serialPort() const;

    DeviceState *m_deviceState;
    CommandInterface *m_rpc;
    State m_state;
};

}
}

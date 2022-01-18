#pragma once

#include <QObject>
#include <QByteArray>

namespace Flipper {
namespace Zero {

class DeviceState;
class CommandInterface;

class VirtualDisplay : public QObject
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

    VirtualDisplay(DeviceState *deviceState, CommandInterface *rpc, QObject *parent = nullptr);

public slots:
    void start(const QByteArray &firstFrame = QByteArray());
    void sendFrame(const QByteArray &screenFrame);
    void stop();

signals:
    void started();
    void stopped();

private:
    void setState(State newState);

    DeviceState *m_deviceState;
    CommandInterface *m_rpc;
    State m_state;
};

}
}


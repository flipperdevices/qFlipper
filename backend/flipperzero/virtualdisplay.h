#pragma once

#include <QObject>
#include <QByteArray>

namespace Flipper {
namespace Zero {

class DeviceState;
class ProtobufSession;

class VirtualDisplay : public QObject
{
    Q_OBJECT

public:
    enum class DisplayState {
        Starting,
        Running,
        Stopping,
        Stopped
    };

    Q_ENUM(DisplayState)

    VirtualDisplay(DeviceState *deviceState, ProtobufSession *rpc, QObject *parent = nullptr);

public slots:
    void start(const QByteArray &firstFrame = QByteArray());
    void sendFrame(const QByteArray &screenFrame);
    void stop();

private:
    void setDisplayState(DisplayState newState);

    DeviceState *m_deviceState;
    ProtobufSession *m_rpc;
    DisplayState m_displayState;
};

}
}


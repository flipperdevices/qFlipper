#pragma once

#include <QObject>
#include <QByteArray>

namespace Flipper {
namespace Zero {

class DeviceState;
class ProtobufSession;

class ScreenStreamer : public QObject
{
    Q_OBJECT

public:
    enum StreamState {
        Starting,
        Running,
        Stopping,
        Stopped
    };

    Q_ENUM(StreamState)

    ScreenStreamer(DeviceState *deviceState, ProtobufSession *rpc, QObject *parent = nullptr);
    ~ScreenStreamer();

    void sendInputEvent(int key, int type);

public slots:
    void start();
    void stop();

    void onBroadcastResponseReceived(QObject *response);

private:
    void sendStopCommand();
    void setStreamState(StreamState newState);

    DeviceState *m_deviceState;
    StreamState m_streamState;
    ProtobufSession *m_rpc;
};

}
}

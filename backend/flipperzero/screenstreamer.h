#pragma once

#include <QSize>
#include <QObject>
#include <QByteArray>

#include "inputevent.h"
#include "screenframe.h"

namespace Flipper {

class FlipperZero;

namespace Zero {

class ScreenStreamer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ScreenFrame screenFrame READ screenFrame NOTIFY screenFrameChanged)
    Q_PROPERTY(bool isEnabled READ isEnabled WRITE setEnabled NOTIFY streamStateChanged)
    Q_PROPERTY(bool isPaused READ isPaused WRITE setPaused NOTIFY streamStateChanged)

public:
    enum StreamState {
        Starting,
        Running,
        Stopping,
        Stopped,
        Paused
    };

    Q_ENUM(StreamState)

    ScreenStreamer(QObject *parent = nullptr);

    void setDevice(FlipperZero *device);
    Q_INVOKABLE void sendInputEvent(InputEvent::Key key, InputEvent::Type type);

    bool isEnabled() const;
    void setEnabled(bool set);

    bool isPaused() const;
    void setPaused(bool set);

    StreamState streamState() const;

    const ScreenFrame &screenFrame() const;

signals:
    void streamStateChanged();
    void screenFrameChanged();

public slots:
    void start();
    void stop();

private slots:
    void onProtobufSessionStateChanged();
    void onBroadcastResponseReceived(QObject *response);

private:
    void setStreamState(StreamState newState);
    void setScreenFrame(const ScreenFrame &frame);

    StreamState m_streamState;
    ScreenFrame m_screenData;
    FlipperZero *m_device;
};

}
}

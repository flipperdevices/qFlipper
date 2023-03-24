#pragma once

#include <QSize>
#include <QObject>
#include <QByteArray>

#include "inputevent.h"

namespace Flipper {

class FlipperZero;

namespace Zero {

class ScreenStreamer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QByteArray screenData READ screenData NOTIFY screenDataChanged)
    Q_PROPERTY(QSize screenSize READ screenSize CONSTANT)
    Q_PROPERTY(bool isEnabled READ isEnabled WRITE setEnabled NOTIFY streamStateChanged)
    Q_PROPERTY(bool isPaused READ isPaused WRITE setPaused NOTIFY streamStateChanged)
    Q_PROPERTY(bool isScreenFlipped READ isScreenFlipped NOTIFY screenFlippedChanged)

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

    bool isScreenFlipped() const;

    StreamState streamState() const;

    static const QSize screenSize();
    const QByteArray &screenData() const;

signals:
    void streamStateChanged();
    void screenDataChanged();
    void screenFlippedChanged();

public slots:
    void start();
    void stop();

private slots:
    void onProtobufSessionStateChanged();
    void onBroadcastResponseReceived(QObject *response);

private:
    void setStreamState(StreamState newState);
    void setScreenData(const QByteArray &data);
    void setScreenFlipped(bool set);

    StreamState m_streamState;
    QByteArray m_screenData;
    FlipperZero *m_device;

    bool m_isScreenFlipped;
};

}
}

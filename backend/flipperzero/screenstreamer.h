#pragma once

#include <QSize>
#include <QObject>
#include <QByteArray>

namespace Flipper {

class FlipperZero;

namespace Zero {

class ScreenStreamer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QByteArray screenData READ screenData NOTIFY screenDataChanged)
    Q_PROPERTY(QSize screenSize READ screenSize CONSTANT)
    Q_PROPERTY(bool isActive READ isActive NOTIFY streamStateChanged)

public:
    enum StreamState {
        Starting,
        Running,
        Stopping,
        Stopped
    };

    Q_ENUM(StreamState)

    ScreenStreamer(QObject *parent = nullptr);

    void setDevice(FlipperZero *device);
    Q_INVOKABLE void sendInputEvent(int key, int type);

    bool isActive() const;
    StreamState streamState() const;

    static const QSize screenSize();
    const QByteArray &screenData() const;

signals:
    void streamStateChanged();
    void screenDataChanged();

public slots:
    void start();
    void stop();

    void onProtobufSessionStateChanged();
    void onBroadcastResponseReceived(QObject *response);

private:
    void setStreamState(StreamState newState);
    void setScreenData(const QByteArray &data);

    StreamState m_streamState;
    QByteArray m_screenData;
    FlipperZero *m_device;
};

}
}

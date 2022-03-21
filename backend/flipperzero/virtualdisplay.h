#pragma once

#include <QObject>
#include <QByteArray>

namespace Flipper {
class FlipperZero;

namespace Zero {

class VirtualDisplay : public QObject
{
    Q_OBJECT

public:
    enum DisplayState {
        Starting,
        Running,
        Stopping,
        Stopped
    };

    Q_ENUM(DisplayState)

    VirtualDisplay(QObject *parent = nullptr);
    void setDevice(FlipperZero *device);

    DisplayState displayState() const;

signals:
    void displayStateChanged();

public slots:
    void start(const QByteArray &firstFrame = QByteArray());
    void sendFrame(const QByteArray &screenFrame);
    void stop();

private slots:
    void onProtobufSessionStateChanged();

private:
    void setDisplayState(DisplayState newState);

    DisplayState m_displayState;
    FlipperZero *m_device;
};

}
}


#pragma once

#include <QObject>
#include <QByteArray>
#include <QSerialPortInfo>

class QSerialPort;

namespace Flipper {
namespace Zero {

class DeviceState;

class ScreenStreamer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QByteArray screenData READ screenData NOTIFY screenDataChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(int screenWidth READ screenWidth CONSTANT)
    Q_PROPERTY(int screenHeight READ screenHeight CONSTANT)

public:
    enum class InputKey {
        Up,
        Down,
        Right,
        Left,
        Ok,
        Back,
    };

    Q_ENUM(InputKey)

    enum class InputType {
        Press, /* Press event, emitted after debounce */
        Release, /* Release event, emitted after debounce */
        Short, /* Short event, emitted after InputTypeRelease done withing INPUT_LONG_PRESS interval */
        Long, /* Long event, emmited after INPUT_LONG_PRESS interval, asynchronouse to InputTypeRelease  */
        Repeat, /* Repeat event, emmited with INPUT_REPEATE_PRESS period after InputTypeLong event */
    };

    Q_ENUM(InputType)

    ScreenStreamer(DeviceState *deviceState, QObject *parent = nullptr);
    ~ScreenStreamer();

    const QByteArray &screenData() const;

    bool isEnabled() const;
    void setEnabled(bool enabled);

    static int screenWidth();
    static int screenHeight();

public slots:
    void sendInputEvent(InputKey key, InputType type);

signals:
    void screenDataChanged();
    void enabledChanged();

private slots:
    void createPort();
    void onPortReadyRead();
    void onPortErrorOccured();

private:
    bool openPort();
    void closePort();

    DeviceState *m_deviceState;
    QSerialPort *m_serialPort;

    QByteArray m_dataBuffer;
    QByteArray m_screenData;

    bool m_isEnabled;
    bool m_isHeaderFound;
};

}
}

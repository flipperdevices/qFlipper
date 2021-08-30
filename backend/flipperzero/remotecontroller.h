#ifndef FLIPPER_ZEROREMOTE_H
#define FLIPPER_ZEROREMOTE_H

#include <QObject>
#include <QByteArray>
#include <QSerialPortInfo>

class QSerialPort;

namespace Flipper {
namespace Zero {

class RemoteController : public QObject
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

    RemoteController(QSerialPort *serialPort, QObject *parent = nullptr);
    ~RemoteController();

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
    void onPortReadyRead();
    void onPortErrorOccured();

private:
    bool openPort();
    void closePort();

    QSerialPort *m_port;
    QByteArray m_dataBuffer;
    QByteArray m_screenData;

    bool m_isEnabled;
    bool m_isHeaderFound;
};

} // namespace Zero
} // namespace Flipper

#endif // FLIPPER_ZEROREMOTE_H

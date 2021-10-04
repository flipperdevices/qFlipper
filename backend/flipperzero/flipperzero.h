#pragma once

#include <QObject>

#include "deviceinfo.h"

namespace Flipper {

namespace Zero {
    class DeviceState;
    class ScreenStreamInterface;
    class FirmwareUpdater;
}

class FlipperZero : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY deviceInfoChanged)
    Q_PROPERTY(QString model READ model CONSTANT)
    Q_PROPERTY(QString target READ target NOTIFY deviceInfoChanged)
    Q_PROPERTY(QString version READ version NOTIFY deviceInfoChanged)

    Q_PROPERTY(QString messageString READ messageString NOTIFY messageChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY isErrorChanged)

    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)

    Q_PROPERTY(bool isDFU READ isDFU NOTIFY deviceInfoChanged)
    Q_PROPERTY(bool isPersistent READ isPersistent NOTIFY isPersistentChanged)
    Q_PROPERTY(bool isOnline READ isOnline NOTIFY isOnlineChanged)
    Q_PROPERTY(bool isError READ isError NOTIFY isErrorChanged)

    Q_PROPERTY(Flipper::Zero::ScreenStreamInterface* screen READ screen CONSTANT)

public:
    FlipperZero(const Zero::DeviceInfo &info, QObject *parent = nullptr);
    ~FlipperZero();

    void reset(const Zero::DeviceInfo &info);
    void setOnline(bool set);

    bool isPersistent() const;
    bool isOnline() const;
    bool isError() const;
    bool isDFU() const;

    const QString &name() const;
    const QString &model() const;
    const QString &target() const;
    const QString &version() const;

    const QString &messageString() const;
    const QString &errorString() const;

    double progress() const;

    const Flipper::Zero::DeviceInfo &deviceInfo() const;

    Flipper::Zero::ScreenStreamInterface *screen() const;
    Flipper::Zero::FirmwareUpdater *updater() const;

public slots:
    void fullUpdate();

signals:
    void deviceInfoChanged();

    void messageChanged();
    void progressChanged();

    void isPersistentChanged();
    void isOnlineChanged();
    void isErrorChanged();

private:
    Zero::DeviceState *m_state;
    Zero::FirmwareUpdater *m_updater;
    Zero::ScreenStreamInterface *m_screen;
};

}

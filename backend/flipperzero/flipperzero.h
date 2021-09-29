#pragma once

#include <QObject>

#include "deviceinfo.h"

namespace Flipper {

namespace Zero {
    class RemoteController;
    class RecoveryController;
    class StorageController;
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

    Q_PROPERTY(Flipper::Zero::RemoteController* remote READ remote CONSTANT)
    Q_PROPERTY(Flipper::Zero::RecoveryController* recovery READ recovery CONSTANT)

public:
    FlipperZero(const Zero::DeviceInfo &info, QObject *parent = nullptr);
    ~FlipperZero();

    void reset(const Zero::DeviceInfo &info);
    void setDeviceInfo(const Zero::DeviceInfo &info);

    void setPersistent(bool set);
    void setOnline(bool set);
    void setError(const QString &msg = QString(), bool set = true);

    bool isPersistent() const;
    bool isOnline() const;
    bool isError() const;
    bool isDFU() const;

    bool bootToDFU();

    const QString &name() const;
    const QString &model() const;
    const QString &target() const;
    const QString &version() const;

    const QString &messageString() const;
    const QString &errorString() const;

    double progress() const;

    const Flipper::Zero::DeviceInfo &deviceInfo() const;

    Flipper::Zero::RemoteController *remote() const;
    Flipper::Zero::RecoveryController *recovery() const;
    Flipper::Zero::StorageController *storage() const;

    void setMessage(const QString &message);
    void setProgress(double progress);

signals:
    void deviceInfoChanged();

    void messageChanged();
    void progressChanged();

    void isPersistentChanged();
    void isOnlineChanged();
    void isErrorChanged();

private slots:
    void onControllerErrorOccured();

private:
    void initControllers();

    bool m_isPersistent;
    bool m_isOnline;
    bool m_isError;

    Zero::DeviceInfo m_deviceInfo;

    QString m_statusMessage;
    QString m_errorString;

    double m_progress;

    Zero::RemoteController *m_remote;
    Zero::RecoveryController *m_recovery;
    Zero::StorageController *m_storage;
};

}

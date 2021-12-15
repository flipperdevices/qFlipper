#pragma once

#include <QObject>

#include "deviceinfo.h"

namespace Flipper {
namespace Zero {

class DeviceState : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Flipper::Zero::DeviceInfo info READ deviceInfo NOTIFY deviceInfoChanged)

    Q_PROPERTY(bool isPersistent READ isPersistent NOTIFY isPersistentChanged)
    Q_PROPERTY(bool isOnline READ isOnline NOTIFY isOnlineChanged)
    Q_PROPERTY(bool isError READ isError NOTIFY errorChanged)
    Q_PROPERTY(bool isRecoveryMode READ isRecoveryMode NOTIFY deviceInfoChanged)

    Q_PROPERTY(QString statusString READ statusString NOTIFY statusChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorChanged)

    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)

public:
    DeviceState(const DeviceInfo &deviceInfo, QObject *parent = nullptr);

    void reset();

    const DeviceInfo &deviceInfo() const;
    void setDeviceInfo(const DeviceInfo &newDeviceInfo);

    bool isPersistent() const;
    void setPersistent(bool set);

    bool isOnline() const;
    void setOnline(bool set);

    bool isError() const;
    void setError(bool set);

    bool isRecoveryMode() const;

    double progress() const;
    void setProgress(double newProgress);

    const QString &statusString() const;
    void setStatusString(const QString &newStatusString);

    const QString &errorString() const;
    void setErrorString(const QString &newErrorString);

    //TODO: Replace with deviceInfo().name
    const QString &name() const;

signals:
    void deviceInfoChanged();
    void isPersistentChanged();
    void isOnlineChanged();

    void updateInfoChanged();

    void statusChanged();
    void errorChanged();

    void progressChanged();

private:
    DeviceInfo m_deviceInfo;

    bool m_isPersistent;
    bool m_isOnline;
    bool m_isError;

    QString m_statusString;
    QString m_errorString;

    double m_progress;
};

}
}


#pragma once

#include <QObject>

#include "deviceinfo.h"

namespace Flipper {
namespace Zero {

class DeviceState : public QObject
{
    Q_OBJECT

public:
    DeviceState(const DeviceInfo &deviceInfo, QObject *parent = nullptr);

    const DeviceInfo &deviceInfo() const;
    void setDeviceInfo(const DeviceInfo &newDeviceInfo);

    bool isPersistent() const;
    void setPersistent(bool set);

    bool isOnline() const;
    void setOnline(bool set);

    bool isError() const;
    void setError(bool set);

    double progress() const;
    void setProgress(double newProgress);

    const QString &statusString() const;
    void setStatusString(const QString &newStatusString);

    const QString &errorString() const;
    void setErrorString(const QString &newErrorString);

signals:
    void deviceInfoChanged();
    void isPersistentChanged();
    void isOnlineChanged();
    void progressChanged();

    void statusChanged();
    void errorChanged();

private:
    DeviceInfo m_deviceInfo;

    bool m_isPersistent;
    bool m_isOnline;
    bool m_isError;

    double m_progress;
    QString m_statusString;
    QString m_errorString;
};

}
}


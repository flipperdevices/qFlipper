#pragma once

#include <QQueue>
#include <QObject>

#include "deviceinfo.h"
#include "backenderror.h"

namespace Flipper {
namespace Zero {

class DeviceState : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Flipper::Zero::DeviceInfo info READ deviceInfo NOTIFY deviceInfoChanged)

    Q_PROPERTY(bool isPersistent READ isPersistent NOTIFY isPersistentChanged)
    Q_PROPERTY(bool isOnline READ isOnline NOTIFY isOnlineChanged)
    Q_PROPERTY(bool isError READ isError NOTIFY isErrorChanged)
    Q_PROPERTY(bool isRecoveryMode READ isRecoveryMode NOTIFY deviceInfoChanged)
    Q_PROPERTY(QString statusString READ statusString NOTIFY statusStringChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY isErrorChanged)
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)

public:
    DeviceState(const DeviceInfo &deviceInfo, QObject *parent = nullptr);

    const DeviceInfo &deviceInfo() const;
    void setDeviceInfo(const DeviceInfo &newDeviceInfo);

    void setStorageInfo(const StorageInfo &newStorageInfo);

    bool isPersistent() const;
    void setPersistent(bool set);

    bool isOnline() const;
    void setOnline(bool set);

    bool isAllowVirtualDisplay() const;
    void setAllowVirtualDisplay(bool set);

    bool isError() const;

    bool isRecoveryMode() const;

    double progress() const;
    void setProgress(double newProgress);

    const QString &statusString() const;
    void setStatusString(const QString &newStatusString);

    const QString &errorString() const;

    BackendError::ErrorType error() const;
    void setError(BackendError::ErrorType error, const QString &errorString);
    void clearError();

    //TODO: Replace with deviceInfo().name
    const QString &name() const;

signals:
    void deviceInfoChanged();
    void isPersistentChanged();
    void isOnlineChanged();
    void statusStringChanged();
    void isErrorChanged();
    void progressChanged();

private slots:
    void onDeviceInfoChanged();
    void onIsOnlineChanged();

private:
    void processQueue();

    DeviceInfo m_deviceInfo;
    QQueue<DeviceInfo> m_queue;

    bool m_isPersistent;
    bool m_isOnline;
    bool m_isAllowVirtualDisplay;

    BackendError::ErrorType m_error;
    QString m_statusString;
    QString m_errorString;

    double m_progress;
};

}
}

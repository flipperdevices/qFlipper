#pragma once

#include <QSize>
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
    Q_PROPERTY(bool isStreamingEnabled READ isStreamingEnabled NOTIFY isStreamingEnabledChanged)
    Q_PROPERTY(bool isVirtualDisplayEnabled READ isVirtualDisplayEnabled NOTIFY isVirtualDisplayEnabledChanged)
    Q_PROPERTY(QString statusString READ statusString NOTIFY statusStringChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY isErrorChanged)
    Q_PROPERTY(QSize screenSize READ screenSize CONSTANT)
    Q_PROPERTY(QByteArray screenData READ screenData NOTIFY screenDataChanged)
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)

public:
    DeviceState(const DeviceInfo &deviceInfo, QObject *parent = nullptr);

    const DeviceInfo &deviceInfo() const;
    void setDeviceInfo(const DeviceInfo &newDeviceInfo);

    bool isPersistent() const;
    void setPersistent(bool set);

    bool isOnline() const;
    void setOnline(bool set);

    bool isError() const;

    bool isRecoveryMode() const;

    bool isStreamingEnabled() const;
    void setStreamingEnabled(bool set);

    bool isVirtualDisplayEnabled() const;
    void setVirtualDisplayEnabled(bool set);

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

    //TODO: Replace with a enum constant?
    static const QSize screenSize();

    const QByteArray &screenData() const;
    void setScreenData(const QByteArray &data);

signals:
    void deviceInfoChanged();
    void isPersistentChanged();
    void isOnlineChanged();
    void isStreamingEnabledChanged();
    void isVirtualDisplayEnabledChanged();
    void statusStringChanged();
    void isErrorChanged();
    void screenDataChanged();
    void progressChanged();

private slots:
    void onDeviceInfoChanged();
    void onIsOnlineChanged();

private:
    void processQueue();

    DeviceInfo m_deviceInfo;
    QQueue<DeviceInfo> m_queue;

    bool m_isPersistent;
    bool m_isStreaming;
    bool m_isVirtualDisplay;
    bool m_isOnline;

    BackendError::ErrorType m_error;
    QString m_statusString;
    QString m_errorString;
    QByteArray m_screenData;

    double m_progress;
};

}
}


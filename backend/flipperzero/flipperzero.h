#pragma once

#include <QSerialPortInfo>
#include <QDateTime>
#include <QObject>
#include <QMutex>

#include "deviceinfo.h"
#include "usbdeviceinfo.h"

class QIODevice;
class QSerialPort;

namespace Flipper {

namespace Zero {
    class RemoteController;
}

// TODO: move Application processor methods into a separate class
// TODO: move Co-Processor core methods into a separate class
class FlipperZero : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY deviceInfoChanged)
    Q_PROPERTY(QString model READ model CONSTANT)
    Q_PROPERTY(QString target READ target NOTIFY deviceInfoChanged)
    Q_PROPERTY(QString version READ version NOTIFY deviceInfoChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)

    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)

    Q_PROPERTY(bool isDFU READ isDFU NOTIFY usbInfoChanged)
    Q_PROPERTY(bool isPersistent READ isPersistent NOTIFY isPersistentChanged)
    Q_PROPERTY(bool isOnline READ isOnline NOTIFY isOnlineChanged)
    Q_PROPERTY(bool isError READ isError NOTIFY isErrorChanged)

    Q_PROPERTY(Flipper::Zero::RemoteController* remote READ remote CONSTANT)

public:
    enum class BootMode {
        Normal,
        DFUOnly
    };

    enum class WirelessStatus {
        WSRunning,
        FUSRunning,
        ErrorOccured,
        UnhandledState,
        Invalid
    };

    FlipperZero(const USBDeviceInfo &info, QObject *parent = nullptr);
    ~FlipperZero();

    void reuse(const FlipperZero *other);

    void setUSBInfo(const USBDeviceInfo &info);
    void setDeviceInfo(const Zero::DeviceInfo &info);

    void setPersistent(bool set);
    void setOnline(bool set);
    void setError(const QString &msg = QString(), bool set = true);

    bool isPersistent() const;
    bool isOnline() const;
    bool isError() const;

    bool bootToDFU();
    bool reboot();

    bool setBootMode(BootMode mode);

    WirelessStatus wirelessStatus();

    bool startFUS();
    bool startWirelessStack();
    bool deleteWirelessStack();
    bool upgradeWirelessStack();

    bool downloadFirmware(QIODevice *file);
    bool downloadOptionBytes(QIODevice *file);
    bool downloadFUS(QIODevice *file, uint32_t addr);
    bool downloadWirelessStack(QIODevice *file, uint32_t addr = 0);

    const QString &name() const;
    const QString &model() const;
    const QString &target() const;
    const QString &version() const;

    const QString &statusMessage() const;

    double progress() const;

    const USBDeviceInfo &usbInfo() const;
    const Flipper::Zero::DeviceInfo &deviceInfo() const;

    bool isDFU() const;

    Flipper::Zero::RemoteController *remote() const;

    void setName(const QString &name);
    void setTarget(const QString &target);
    void setVersion(const QString &version);
    void setStatusMessage(const QString &message);
    void setProgress(double progress);

signals:
    void usbInfoChanged();
    void deviceInfoChanged();

    void statusMessageChanged(const QString&);
    void progressChanged(double);

    void isPersistentChanged();
    void isOnlineChanged();
    void isErrorChanged();

private slots:
    void initVCPMode(const QSerialPortInfo &portInfo);
    void fetchDeviceInfo();

private:
    void setSerialPort(QSerialPort *serialPort);
    void setRemoteController(Zero::RemoteController *remote);

    void statusFeedback(const char *msg);
    void errorFeedback(const char *msg);

    bool m_isPersistent;
    bool m_isOnline;
    bool m_isError;

    USBDeviceInfo m_usbInfo;
    Zero::DeviceInfo m_deviceInfo;
    QMutex m_deviceMutex;

    QString m_statusMessage;
    double m_progress;

    QSerialPort *m_serialPort;
    Zero::RemoteController *m_remote;
};

}

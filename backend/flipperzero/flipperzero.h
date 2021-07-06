#ifndef FLIPPERZERO_H
#define FLIPPERZERO_H

#include <QObject>
#include <QMutex>

#include "usbdeviceinfo.h"

class QIODevice;
class QSerialPort;

namespace Flipper {

namespace Zero {
    class RemoteController;
}

class FlipperZero : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString model READ model CONSTANT)
    Q_PROPERTY(QString target READ target NOTIFY targetChanged)
    Q_PROPERTY(QString version READ version NOTIFY versionChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(bool isDFU READ isDFU NOTIFY isDFUChanged)
    Q_PROPERTY(Flipper::Zero::RemoteController* remote READ remote CONSTANT)

public:
    enum class BootMode {
        Normal,
        DFUOnly
    };

    FlipperZero(const USBDeviceInfo &info, QObject *parent = nullptr);

    void setDeviceInfo(const USBDeviceInfo &info);
    void setPersistent(bool set);
    void setConnected(bool set);

    bool isPersistent() const;
    bool isConnected() const;

    bool detach();
    bool setBootMode(BootMode mode);

    bool downloadFirmware(QIODevice *file);
    bool downloadFUS(QIODevice *file, uint32_t addr);
    bool downloadRadioStack(QIODevice *file, uint32_t addr);

    const QString &name() const;
    const QString &model() const;
    const QString &target() const;
    const QString &version() const;
    const QString &statusMessage() const;

    double progress() const;

    const USBDeviceInfo &info() const;

    bool isDFU() const;

    Flipper::Zero::RemoteController *remote() const;

    void setName(const QString &name);
    void setTarget(const QString &target);
    void setVersion(const QString &version);
    void setStatusMessage(const QString &message);
    void setProgress(double progress);

signals:
    void nameChanged(const QString&);
    void targetChanged(const QString&);
    void versionChanged(const QString&);
    void statusMessageChanged(const QString&);
    void progressChanged(double);

    void isDFUChanged();

private:
    void fetchInfoVCPMode();
    void fetchInfoDFUMode();

    bool m_isPersistent;
    bool m_isConnected;

    USBDeviceInfo m_info;
    QMutex m_deviceMutex;

    QString m_name;
    QString m_target;
    QString m_version;
    QString m_statusMessage;

    double m_progress;

    Zero::RemoteController *m_remote;
};

}

#endif // FLIPPERZERO_H

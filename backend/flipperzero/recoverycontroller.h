#pragma once

#include <QObject>

#include "usbdeviceinfo.h"

class QIODevice;

namespace Flipper {
namespace Zero {

class RecoveryController : public QObject
{
    Q_OBJECT

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

    RecoveryController(USBDeviceInfo info, QObject *parent = nullptr);
    ~RecoveryController();

    const QString &message() const;
    const QString &errorString() const;

    WirelessStatus wirelessStatus();
    double progress() const;
    bool isError() const;

    bool leaveDFU();
    bool setBootMode(BootMode mode);

    bool startFUS();
    bool startWirelessStack();
    bool deleteWirelessStack();
    bool upgradeWirelessStack();

    bool downloadFirmware(QIODevice *file);
    bool downloadOptionBytes(QIODevice *file);
    bool downloadWirelessStack(QIODevice *file, uint32_t addr = 0);

signals:
    void messageChanged();
    void errorOccured();
    void progressChanged();

private:
    void setProgress(double progress);
    void setMessage(const QString &msg);
    void setError(const QString &msg);

    USBDeviceInfo m_usbInfo;

    bool m_isError;
    QString m_message;
    QString m_errorString;
    double m_progress;
};

}
}

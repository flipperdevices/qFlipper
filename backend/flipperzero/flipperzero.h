#pragma once

#include <QSize>
#include <QObject>

class USBDeviceInfo;
class AbstractOperation;

namespace Flipper {
namespace Updates {
class VersionInfo;
}

namespace Zero {
    struct DeviceInfo;
    class DeviceState;
    class ProtobufSession;
    class RecoveryInterface;
    class UtilityInterface;
}

class FlipperZero : public QObject
{
    Q_OBJECT

public:
    FlipperZero(const Zero::DeviceInfo &info, QObject *parent = nullptr);

    Zero::DeviceState *deviceState() const;
    Zero::ProtobufSession *rpc() const;
    Zero::UtilityInterface *utility() const;

    bool canUpdate(const Flipper::Updates::VersionInfo &versionInfo) const;
    bool canInstall(const Flipper::Updates::VersionInfo &versionInfo) const;
    bool canRepair(const Flipper::Updates::VersionInfo &versionInfo) const;

    void fullUpdate(const Flipper::Updates::VersionInfo &versionInfo);
    void fullRepair(const Flipper::Updates::VersionInfo &versionInfo);

    void createBackup(const QUrl &backupUrl);
    void restoreBackup(const QUrl &backupUrl);
    void factoryReset();

    void installFirmware(const QUrl &fileUrl);
    void installWirelessStack(const QUrl &fileUrl);
    void installFUS(const QUrl &fileUrl, uint32_t address);

    void refreshStorageInfo();
    void finalizeOperation();

signals:
    void deviceStateChanged();
    void operationFinished();

private slots:
    void onDeviceInfoChanged();
    void onSessionStatusChanged();

private:
    void registerOperation(AbstractOperation *operation);

    Zero::DeviceState *m_state;
    Zero::ProtobufSession *m_rpc;
    Zero::RecoveryInterface *m_recovery;
    Zero::UtilityInterface *m_utility;
};

}

#pragma once

#include <QSize>
#include <QObject>

class AbstractOperation;

namespace Flipper {
namespace Updates {
class VersionInfo;
}

namespace Zero {
    struct DeviceInfo;
    class DeviceState;
    class CommandInterface;
    class RecoveryInterface;
    class UtilityInterface;
    class ScreenStreamer;
}

class FlipperZero : public QObject
{
    Q_OBJECT

public:
    FlipperZero(const Zero::DeviceInfo &info, QObject *parent = nullptr);
    ~FlipperZero();

    Zero::DeviceState *deviceState() const;

    bool canUpdate(const Flipper::Updates::VersionInfo &versionInfo) const;
    bool canInstall(const Flipper::Updates::VersionInfo &versionInfo) const;
    bool canRepair(const Flipper::Updates::VersionInfo &versionInfo) const;

    void fullUpdate(const Flipper::Updates::VersionInfo &versionInfo);
    void fullRepair(const Flipper::Updates::VersionInfo &versionInfo);

    void createBackup(const QUrl &directoryUrl);
    void restoreBackup(const QUrl &directoryUrl);
    void factoryReset();

    void installFirmware(const QUrl &fileUrl);
    void installWirelessStack(const QUrl &fileUrl);
    void installFUS(const QUrl &fileUrl, uint32_t address);

    void sendInputEvent(int key, int type);
    void finalizeOperation();

signals:
    void stateChanged();
    void operationFinished();

private slots:
    void onIsOnlineChanged();

private:
    void registerOperation(AbstractOperation *operation);

    Zero::DeviceState *m_state;
    Zero::CommandInterface *m_rpc;
    Zero::RecoveryInterface *m_recovery;
    Zero::UtilityInterface *m_utility;
    Zero::ScreenStreamer *m_streamer;
};

}

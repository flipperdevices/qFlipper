#pragma once

#include <QObject>

#include "deviceinfo.h"
#include "flipperupdates.h"

class AbstractOperation;

namespace Flipper {

namespace Zero {
    class DeviceState;
    class CommandInterface;
    class RecoveryInterface;
    class UtilityInterface;
    class ScreenStreamer;
    class FirmwareUpdater;
}

class FlipperZero : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Flipper::Zero::DeviceState* state READ deviceState CONSTANT)
    Q_PROPERTY(Flipper::Zero::FirmwareUpdater* updater READ updater CONSTANT)
    Q_PROPERTY(Flipper::Zero::ScreenStreamer* streamer READ streamer CONSTANT)

public:
    FlipperZero(const Zero::DeviceInfo &info, QObject *parent = nullptr);
    ~FlipperZero();

    void updateOrRepair(const Flipper::Updates::VersionInfo &versionInfo);

    void fullUpdate(const Flipper::Updates::VersionInfo &versionInfo);
    void fullRepair(const Flipper::Updates::VersionInfo &versionInfo);

    void createBackup(const QUrl &directoryUrl);
    void restoreBackup(const QUrl &directoryUrl);
    void factoryReset();

    void installFirmware(const QUrl &fileUrl);
    void installWirelessStack(const QUrl &fileUrl);
    void installFUS(const QUrl &fileUrl, uint32_t address);

    Flipper::Zero::DeviceState *deviceState() const;
    Flipper::Zero::ScreenStreamer *streamer() const;
    Flipper::Zero::FirmwareUpdater *updater() const;

signals:
    void operationFinished();

private slots:
    void onStreamConditionChanged();

private:
    void registerOperation(AbstractOperation *operation);

    Zero::DeviceState *m_state;
    Zero::CommandInterface *m_rpc;
    Zero::RecoveryInterface *m_recovery;
    Zero::UtilityInterface *m_utility;
    Zero::FirmwareUpdater *m_updater;
    Zero::ScreenStreamer *m_streamer;
};

}

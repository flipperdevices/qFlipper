#pragma once

#include "flipperupdates.h"
#include "abstractoperationrunner.h"

namespace Flipper {
namespace Zero {

class DeviceState;
class RecoveryInterface;
class UtilityInterface;
class CommandInterface;

class FirmwareUpdater : public AbstractOperationRunner
{
    Q_OBJECT

    enum class ChannelType {
        Development = 0,
        ReleaseCandidate,
        Release
    };

public:
    FirmwareUpdater(DeviceState *state, CommandInterface *rpc, QObject *parent = nullptr);

public slots:
    bool canUpdate(const Flipper::Updates::VersionInfo &versionInfo) const;
    bool canInstall() const;

    void fullUpdate(const Flipper::Updates::VersionInfo &versionInfo);
    void fullRepair(const Flipper::Updates::VersionInfo &versionInfo);

    void backupInternalStorage(const QUrl &directoryUrl);
    void restoreInternalStorage(const QUrl &directoryUrl);
    void factoryReset();

    void localFirmwareInstall(const QUrl &fileUrl);
    void localFUSUpdate(const QUrl &fileUrl);
    void localWirelessStackUpdate(const QUrl &fileUrl);

private:
    const QLoggingCategory &loggingCategory() const override;

    static const QString &channelName(ChannelType channelType);
    const QString &branchToChannelName() const;
    ChannelType branchToChannelType() const;

    DeviceState *m_state;
    RecoveryInterface *m_recovery;
    UtilityInterface *m_utility;
};

}
}


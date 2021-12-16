#pragma once

#include "flipperupdates.h"
#include "abstractoperationrunner.h"

namespace Flipper {
namespace Zero {

class DeviceState;
class RecoveryInterface;
class UtilityInterface;
class CommandInterface;
class AbstractTopLevelOperation;

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

    Q_INVOKABLE bool canUpdate(const Flipper::Updates::VersionInfo &versionInfo) const;
    Q_INVOKABLE bool canInstall() const;

    AbstractTopLevelOperation *fullUpdate(const Flipper::Updates::VersionInfo &versionInfo);
    AbstractTopLevelOperation *fullRepair(const Flipper::Updates::VersionInfo &versionInfo);

    AbstractTopLevelOperation *backupInternalStorage(const QUrl &directoryUrl);
    AbstractTopLevelOperation *restoreInternalStorage(const QUrl &directoryUrl);
    AbstractTopLevelOperation *factoryReset();

    AbstractTopLevelOperation *localFirmwareInstall(const QUrl &fileUrl);
    AbstractTopLevelOperation *localFUSUpdate(const QUrl &fileUrl, uint32_t address);
    AbstractTopLevelOperation *localWirelessStackUpdate(const QUrl &fileUrl);

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


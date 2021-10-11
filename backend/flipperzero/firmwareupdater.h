#pragma once

#include "flipperupdates.h"
#include "abstractoperationrunner.h"

namespace Flipper {
namespace Zero {

class DeviceState;
class RecoveryInterface;
class UtilityInterface;

class FirmwareUpdater : public AbstractOperationRunner
{
    Q_OBJECT

    enum class ChannelType {
        Development = 0,
        ReleaseCandidate,
        Release
    };

public:
    FirmwareUpdater(DeviceState *state, QObject *parent = nullptr);

public slots:
    bool canUpdate(const Flipper::Updates::VersionInfo &versionInfo) const;
    bool canRollback(const Flipper::Updates::VersionInfo &versionInfo) const;
    bool canChangeChannel() const;

    void fullUpdate(const Flipper::Updates::VersionInfo &versionInfo);
    void fullRepair(const Flipper::Updates::VersionInfo &versionInfo);

    void localFirmwareUpdate(const QUrl &fileUrl);
    void localFUSUpdate(const QUrl &fileUrl);
    void localWirelessStackUpdate(const QUrl &fileUrl);

private:
    static const QString &channelName(ChannelType channelType);
    const QString &branchToChannelName() const;

    DeviceState *m_state;
    RecoveryInterface *m_recovery;
    UtilityInterface *m_utility;
};

}
}


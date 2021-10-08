#include "firmwareupdater.h"

#include "devicestate.h"
#include "updateregistry.h"
#include "flipperupdates.h"
#include "recoveryinterface.h"
#include "utilityinterface.h"

#include "toplevel/fullupdateoperation.h"

#include "preferences.h"
#include "macros.h"

using namespace Flipper;
using namespace Zero;

FirmwareUpdater::FirmwareUpdater(DeviceState *state, QObject *parent):
    AbstractOperationRunner(parent),
    m_state(state),
    m_recovery(new RecoveryInterface(state, this)),
    m_utility(new UtilityInterface(state, this))
{
    connect(m_state, &DeviceState::deviceInfoChanged, this, &FirmwareUpdater::updateInfoChanged);
    connect(UpdateRegistry::firmwareUpdates(), &UpdateRegistry::channelsChanged, this, &FirmwareUpdater::updateInfoChanged);
    connect(Preferences::instance(), &Preferences::firmwareUpdateChannelChanged, this, &FirmwareUpdater::updateInfoChanged);
}

bool FirmwareUpdater::isReady() const
{
    return UpdateRegistry::firmwareUpdates()->channelNames().size();
}

void FirmwareUpdater::fullUpdate()
{
    const auto currentChannel = Preferences::instance()->firmwareUpdateChannel();
    const auto &latestVersion = UpdateRegistry::firmwareUpdates()->channel(currentChannel).latestVersion();
    auto *operation = new FullUpdateOperation(m_recovery, m_utility, m_state, latestVersion, this);
    enqueueOperation(operation);
}

bool FirmwareUpdater::canUpdate() const
{
    if(canChangeChannel()) {
        return false;
    }

    const auto &deviceChannelName = branchToChannelName();
    check_return_bool(UpdateRegistry::firmwareUpdates()->channelNames().contains(deviceChannelName), "Specified update channel not found.");

    const auto &channel = UpdateRegistry::firmwareUpdates()->channel(deviceChannelName);
    const auto &latestVersionNumber = channel.latestVersion().number();

    if(deviceChannelName == channelName(ChannelType::Development)) {
        return m_state->deviceInfo().firmware.commit != latestVersionNumber;
    } else {
        return m_state->deviceInfo().firmware.version < latestVersionNumber;
    }
}

bool FirmwareUpdater::canRollback() const
{
    if(canChangeChannel()) {
        return false;
    }

    const auto &deviceChannelName = branchToChannelName();
    check_return_bool(UpdateRegistry::firmwareUpdates()->channelNames().contains(deviceChannelName), "Specified update channel not found.");

    const auto &channel = UpdateRegistry::firmwareUpdates()->channel(deviceChannelName);
    const auto &latestVersionNumber = channel.latestVersion().number();

    if(deviceChannelName == channelName(ChannelType::Development)) {
        return false;
    } else {
        return m_state->deviceInfo().firmware.version > latestVersionNumber;
    }
}

bool FirmwareUpdater::canChangeChannel() const
{
    const auto currentChannel = Preferences::instance()->firmwareUpdateChannel();
    return branchToChannelName() != currentChannel;
}

const QString &FirmwareUpdater::channelName(ChannelType channelType)
{
    static const QStringList channelNames = {
        QStringLiteral("development"), QStringLiteral("release-candidate"), QStringLiteral("release")
    };

    return channelNames[(int)channelType];
}

const QString &FirmwareUpdater::branchToChannelName() const
{
    const auto &branchName = m_state->deviceInfo().firmware.branch;

    if(branchName == QStringLiteral("dev")) {
        return channelName(ChannelType::Development);
    } else if(branchName.contains(QStringLiteral("-rc"))) {
        return channelName(ChannelType::ReleaseCandidate);
    } else {
        return channelName(ChannelType::Release);
    }
}

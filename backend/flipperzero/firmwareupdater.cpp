#include "firmwareupdater.h"

#include "devicestate.h"
#include "preferences.h"

using namespace Flipper;
using namespace Zero;

FirmwareUpdater::FirmwareUpdater(DeviceState *state, QObject *parent):
    QObject(parent),
    m_state(state)
{}

bool FirmwareUpdater::canUpdate(const Updates::VersionInfo &versionInfo) const
{
    const auto &storageInfo = m_state->deviceInfo().storage;
    if(storageInfo.isExternalPresent && !storageInfo.isAssetsInstalled) {
        return true;
    }

    const auto &deviceChannel = branchToChannelName();
    const auto &deviceVersion = (deviceChannel == channelName(ChannelType::Development)) ?
                m_state->deviceInfo().firmware.commit :
                m_state->deviceInfo().firmware.version;

    const auto &deviceDate = m_state->deviceInfo().firmware.date;

    const auto &serverChannel = globalPrefs->firmwareUpdateChannel();
    const auto &serverVersion = versionInfo.number();
    const auto &serverDate = versionInfo.date();

    if(deviceChannel == channelName(ChannelType::Release)) {
        if(serverChannel == channelName(ChannelType::Release)) {
            return deviceVersion < serverVersion;
        } else if(serverChannel == channelName(ChannelType::ReleaseCandidate)) {
            return deviceVersion < serverVersion.chopped(serverVersion.length() - deviceVersion.length());
        } else if(serverChannel == channelName(ChannelType::Development)) {
            return deviceDate <= serverDate;
        }

    } else if(deviceChannel == channelName(ChannelType::ReleaseCandidate)) {
        if(serverChannel == channelName(ChannelType::Release)) {
            return deviceVersion.chopped(deviceVersion.length() - serverVersion.length()) <= serverVersion;
        } else if(serverChannel == channelName(ChannelType::ReleaseCandidate)) {
            return deviceVersion < serverVersion;
        } else if(serverChannel == channelName(ChannelType::Development)) {
            return deviceDate <= serverDate;
        }

    } else if(deviceChannel == channelName(ChannelType::Development)) {
        if(serverChannel == channelName(ChannelType::Release)) {
            return deviceDate <= serverDate;
        } else if(serverChannel == channelName(ChannelType::ReleaseCandidate)) {
            return deviceDate <= serverDate;
        } else if(serverChannel == channelName(ChannelType::Development)) {
            return (deviceVersion != serverVersion) && (deviceDate <= serverDate);
        }
    }

    return false;
}

bool FirmwareUpdater::canInstall() const
{
    const auto &deviceChannel = branchToChannelName();
    const auto &serverChannel = globalPrefs->firmwareUpdateChannel();

    return deviceChannel != serverChannel;
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

FirmwareUpdater::ChannelType FirmwareUpdater::branchToChannelType() const
{
    const auto &branchName = m_state->deviceInfo().firmware.branch;

    if(branchName == QStringLiteral("dev")) {
        return ChannelType::Development;
    } else if(branchName.contains(QStringLiteral("-rc"))) {
        return ChannelType::ReleaseCandidate;
    } else {
        return ChannelType::Release;
    }
}

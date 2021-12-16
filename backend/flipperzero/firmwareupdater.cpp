#include "firmwareupdater.h"

#include <QLoggingCategory>

#include "devicestate.h"
#include "recoveryinterface.h"
#include "utilityinterface.h"

#include "toplevel/wirelessstackupdateoperation.h"
#include "toplevel/firmwareinstalloperation.h"
#include "toplevel/settingsrestoreoperation.h"
#include "toplevel/settingsbackupoperation.h"
#include "toplevel/factoryresetoperation.h"
#include "toplevel/fullrepairoperation.h"
#include "toplevel/fullupdateoperation.h"

#include "preferences.h"

Q_LOGGING_CATEGORY(CATEGORY_TOPLEVEL, "TOPLEVEL")

using namespace Flipper;
using namespace Zero;

FirmwareUpdater::FirmwareUpdater(DeviceState *state, CommandInterface *rpc, QObject *parent):
    AbstractOperationRunner(parent),
    m_state(state),
    m_recovery(new RecoveryInterface(state, this)),
    m_utility(new UtilityInterface(state, rpc, this))
{}

AbstractTopLevelOperation *FirmwareUpdater::fullUpdate(const Updates::VersionInfo &versionInfo)
{
    return registerOperation(new FullUpdateOperation(m_recovery, m_utility, m_state, versionInfo, this));
}

AbstractTopLevelOperation *FirmwareUpdater::fullRepair(const Updates::VersionInfo &versionInfo)
{
    return registerOperation(new FullRepairOperation(m_recovery, m_utility, m_state, versionInfo, this));
}

AbstractTopLevelOperation *FirmwareUpdater::backupInternalStorage(const QUrl &directoryUrl)
{
    return registerOperation(new SettingsBackupOperation(m_utility, m_state, directoryUrl, this));
}

AbstractTopLevelOperation *FirmwareUpdater::restoreInternalStorage(const QUrl &directoryUrl)
{
    return registerOperation(new SettingsRestoreOperation(m_utility, m_state, directoryUrl, this));
}

AbstractTopLevelOperation *FirmwareUpdater::factoryReset()
{
    return registerOperation(new FactoryResetOperation(m_utility, m_state, this));
}

AbstractTopLevelOperation *FirmwareUpdater::localFirmwareInstall(const QUrl &fileUrl)
{
    return registerOperation(new FirmwareInstallOperation(m_recovery, m_utility, m_state, fileUrl.toLocalFile(), this));
}

AbstractTopLevelOperation *FirmwareUpdater::localFUSUpdate(const QUrl &fileUrl, uint32_t address)
{
    //TODO: User-settable address 0x080EC000
    return registerOperation(new FUSUpdateOperation(m_recovery, m_utility, m_state, fileUrl.toLocalFile(), address, this));
}

AbstractTopLevelOperation *FirmwareUpdater::localWirelessStackUpdate(const QUrl &fileUrl)
{
    return registerOperation(new WirelessStackUpdateOperation(m_recovery, m_utility, m_state, fileUrl.toLocalFile(), this));
}

const QLoggingCategory &FirmwareUpdater::loggingCategory() const
{
    return CATEGORY_TOPLEVEL();
}

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

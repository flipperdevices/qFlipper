#include "firmwareupdater.h"

#include "devicestate.h"
#include "recoveryinterface.h"
#include "utilityinterface.h"

#include "toplevel/wirelessstackupdateoperation.h"
#include "toplevel/firmwareupdateoperation.h"
#include "toplevel/fullrepairoperation.h"
#include "toplevel/fullupdateoperation.h"

#include "preferences.h"

using namespace Flipper;
using namespace Zero;

FirmwareUpdater::FirmwareUpdater(DeviceState *state, QObject *parent):
    AbstractOperationRunner(parent),
    m_state(state),
    m_recovery(new RecoveryInterface(state, this)),
    m_utility(new UtilityInterface(state, this))
{}

void FirmwareUpdater::fullUpdate(const Updates::VersionInfo &versionInfo)
{
    if(m_state->isRecoveryMode()) {
        return;
    }

    auto *operation = new FullUpdateOperation(m_recovery, m_utility, m_state, versionInfo, this);
    enqueueOperation(operation);
}

void FirmwareUpdater::fullRepair(const Updates::VersionInfo &versionInfo)
{
    if(!m_state->isRecoveryMode()) {
        return;
    }

    auto *operation = new FullRepairOperation(m_recovery, m_utility, m_state, versionInfo, this);
    enqueueOperation(operation);
}

void FirmwareUpdater::localFirmwareUpdate(const QUrl &fileUrl)
{
    enqueueOperation(new FirmwareUpdateOperation(m_recovery, m_utility, m_state, fileUrl.toLocalFile(), this));
}

void FirmwareUpdater::localFUSUpdate(const QUrl &fileUrl)
{
    //TODO: User-settable address
    enqueueOperation(new FUSUpdateOperation(m_recovery, m_utility, m_state, fileUrl.toLocalFile(), 0x080EC000, this));
}

void FirmwareUpdater::localWirelessStackUpdate(const QUrl &fileUrl)
{
    enqueueOperation(new WirelessStackUpdateOperation(m_recovery, m_utility, m_state, fileUrl.toLocalFile(), this));
}

bool FirmwareUpdater::canUpdate(const Updates::VersionInfo &versionInfo) const
{
    if(canChangeChannel()) {
        return false;
    } else if(branchToChannelName() == channelName(ChannelType::Development)) {
        return m_state->deviceInfo().firmware.commit != versionInfo.number();
    } else {
        return m_state->deviceInfo().firmware.version < versionInfo.number();
    }
}

bool FirmwareUpdater::canRollback(const Updates::VersionInfo &versionInfo) const
{
    if(canChangeChannel() || (branchToChannelName() == channelName(ChannelType::Development))) {
        return false;
    } else {
        return m_state->deviceInfo().firmware.version > versionInfo.number();
    }
}

bool FirmwareUpdater::canChangeChannel() const
{
    return branchToChannelName() != globalPrefs()->firmwareUpdateChannel();
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

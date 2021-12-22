#include "flipperzero.h"

#include "preferences.h"
#include "flipperupdates.h"

#include "devicestate.h"
#include "screenstreamer.h"

#include "commandinterface.h"
#include "utilityinterface.h"
#include "recoveryinterface.h"

#include "toplevel/wirelessstackupdateoperation.h"
#include "toplevel/firmwareinstalloperation.h"
#include "toplevel/settingsrestoreoperation.h"
#include "toplevel/settingsbackupoperation.h"
#include "toplevel/factoryresetoperation.h"
#include "toplevel/fullrepairoperation.h"
#include "toplevel/fullupdateoperation.h"

#include "preferences.h"

#define CHANNEL_DEVELOPMENT "development"
#define CHANNEL_RELEASE_CANDIDATE "release-candidate"
#define CHANNEL_RELEASE "release"

using namespace Flipper;
using namespace Zero;

FlipperZero::FlipperZero(const Zero::DeviceInfo &info, QObject *parent):
    QObject(parent),
    m_state(new DeviceState(info, this)),
    m_rpc(new CommandInterface(m_state, this)),
    m_recovery(new RecoveryInterface(m_state, this)),
    m_utility(new UtilityInterface(m_state, m_rpc, this)),
    m_streamer(new ScreenStreamer(m_rpc, this))
{
    connect(m_state, &DeviceState::isPersistentChanged, this, &FlipperZero::onStreamConditionChanged);
    connect(m_state, &DeviceState::isOnlineChanged, this, &FlipperZero::onStreamConditionChanged);

    // Add other connections as necessary.
    connect(m_state, &DeviceState::deviceInfoChanged, this, &FlipperZero::stateChanged);
}

FlipperZero::~FlipperZero()
{
    m_state->setOnline(false);
}

bool FlipperZero::canUpdate(const Updates::VersionInfo &versionInfo) const
{
    const auto &storageInfo = m_state->deviceInfo().storage;
    if(storageInfo.isExternalPresent && !storageInfo.isAssetsInstalled) {
        return true;
    }

    static const auto DEVELOPMENT = QStringLiteral("development");
    static const auto RELEASE_CANDIDATE = QStringLiteral("release-candidate");
    static const auto RELEASE = QStringLiteral("release");

    const auto &firmwareInfo = m_state->deviceInfo().firmware;

    const auto &deviceChannel = firmwareInfo.channel;
    const auto &deviceVersion = (deviceChannel == QStringLiteral("development")) ?
                firmwareInfo.commit :
                firmwareInfo.version;

    const auto &deviceDate = firmwareInfo.date;

    const auto &serverChannel = globalPrefs->firmwareUpdateChannel();
    const auto &serverVersion = versionInfo.number();
    const auto &serverDate = versionInfo.date();

    if(deviceChannel == RELEASE) {
        if(serverChannel == RELEASE) {
            return deviceVersion < serverVersion;
        } else if(serverChannel == RELEASE_CANDIDATE) {
            return deviceVersion < serverVersion.chopped(serverVersion.length() - deviceVersion.length());
        } else if(serverChannel == DEVELOPMENT) {
            return deviceDate <= serverDate;
        }

    } else if(deviceChannel == RELEASE_CANDIDATE) {
        if(serverChannel == RELEASE) {
            return deviceVersion.chopped(deviceVersion.length() - serverVersion.length()) <= serverVersion;
        } else if(serverChannel == RELEASE_CANDIDATE) {
            return deviceVersion < serverVersion;
        } else if(serverChannel == DEVELOPMENT) {
            return deviceDate <= serverDate;
        }

    } else if(deviceChannel == DEVELOPMENT) {
        if(serverChannel == RELEASE) {
            return deviceDate <= serverDate;
        } else if(serverChannel == RELEASE_CANDIDATE) {
            return deviceDate <= serverDate;
        } else if(serverChannel == DEVELOPMENT) {
            return (deviceVersion != serverVersion) && (deviceDate <= serverDate);
        }
    }

    return false;
}

bool FlipperZero::canInstall(const Updates::VersionInfo &versionInfo) const
{
    Q_UNUSED(versionInfo)

    const auto &deviceChannel = m_state->deviceInfo().firmware.channel;
    const auto &serverChannel = globalPrefs->firmwareUpdateChannel();

    return deviceChannel != serverChannel;
}

bool FlipperZero::canRepair(const Updates::VersionInfo &versionInfo) const
{
    Q_UNUSED(versionInfo)
    return m_state->isRecoveryMode();
}

void FlipperZero::fullUpdate(const Updates::VersionInfo &versionInfo)
{
    registerOperation(new FullUpdateOperation(m_recovery, m_utility, m_state, versionInfo, this));
}

void FlipperZero::fullRepair(const Updates::VersionInfo &versionInfo)
{
    registerOperation(new FullRepairOperation(m_recovery, m_utility, m_state, versionInfo, this));
}

void FlipperZero::createBackup(const QUrl &directoryUrl)
{
    registerOperation(new SettingsBackupOperation(m_utility, m_state, directoryUrl, this));
}

void FlipperZero::restoreBackup(const QUrl &directoryUrl)
{
    registerOperation(new SettingsRestoreOperation(m_utility, m_state, directoryUrl, this));
}

void FlipperZero::factoryReset()
{
    registerOperation(new FactoryResetOperation(m_utility, m_state, this));
}

void FlipperZero::installFirmware(const QUrl &fileUrl)
{
    registerOperation(new FirmwareInstallOperation(m_recovery, m_utility, m_state, fileUrl.toLocalFile(), this));
}

void FlipperZero::installWirelessStack(const QUrl &fileUrl)
{
    registerOperation(new WirelessStackUpdateOperation(m_recovery, m_utility, m_state, fileUrl.toLocalFile(), this));
}

void FlipperZero::installFUS(const QUrl &fileUrl, uint32_t address)
{
    registerOperation(new FUSUpdateOperation(m_recovery, m_utility, m_state, fileUrl.toLocalFile(), address, this));
}

DeviceState *FlipperZero::deviceState() const
{
    return m_state;
}

Flipper::Zero::ScreenStreamer *FlipperZero::streamer() const
{
    return m_streamer;
}

void FlipperZero::onStreamConditionChanged()
{
    // Automatically start screen streaming if the conditions are right:
    // 1. There is no error
    // 2. Device is online and connected in VCP mode
    // 3. There is no ongoing operation

    const auto streamCondition = m_state->isOnline() &&
            !(m_state->isError() || m_state->isRecoveryMode() || m_state->isPersistent());

    if(streamCondition) {
        m_streamer->start();
    }
}

void FlipperZero::registerOperation(AbstractOperation *operation)
{
    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            m_state->setErrorString(operation->errorString());
        }

        operation->deleteLater();
        emit operationFinished();
    });

    if(m_state->isRecoveryMode()) {
        operation->start();

    } else {
        connect(m_streamer, &ScreenStreamer::stopped, operation, [=]() {
            //TODO: Check that ScreenStreamer has correctly stopped
            operation->start();
        });

        m_streamer->stop();
    }
}

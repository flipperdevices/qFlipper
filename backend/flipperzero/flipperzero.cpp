#include "flipperzero.h"

#include "devicestate.h"
#include "screenstreamer.h"
#include "firmwareupdater.h"

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

using namespace Flipper;
using namespace Zero;

FlipperZero::FlipperZero(const Zero::DeviceInfo &info, QObject *parent):
    QObject(parent),
    m_state(new DeviceState(info, this)),
    m_rpc(new CommandInterface(m_state, this)),
    m_recovery(new RecoveryInterface(m_state, this)),
    m_utility(new UtilityInterface(m_state, m_rpc, this)),
    m_updater(new FirmwareUpdater(m_state, this)),
    m_streamer(new ScreenStreamer(m_rpc, this))
{
    connect(m_state, &DeviceState::isPersistentChanged, this, &FlipperZero::onStreamConditionChanged);
    connect(m_state, &DeviceState::isOnlineChanged, this, &FlipperZero::onStreamConditionChanged);
}

FlipperZero::~FlipperZero()
{
    m_state->setOnline(false);
}

void FlipperZero::updateOrRepair(const Updates::VersionInfo &versionInfo)
{
    if(m_state->isRecoveryMode()) {
        registerOperation(new FullRepairOperation(m_recovery, m_utility, m_state, versionInfo, this));
    } else {
        registerOperation(new FullUpdateOperation(m_recovery, m_utility, m_state, versionInfo, this));
    }
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

FirmwareUpdater *FlipperZero::updater() const
{
    return m_updater;
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

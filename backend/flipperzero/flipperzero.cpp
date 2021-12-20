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
    qDebug().noquote() << "Creating backup in" << directoryUrl << "...";
}

void FlipperZero::restoreBackup(const QUrl &directoryUrl)
{
    qDebug().noquote() << "Restoring backup from" << directoryUrl << "...";
}

void FlipperZero::factoryReset()
{
    qDebug() << "Executing factory reset...";
}

void FlipperZero::installFirmware(const QUrl &fileUrl)
{
    qDebug().noquote() << "Installing firmware from" << fileUrl << "...";
}

void FlipperZero::installWirelessStack(const QUrl &fileUrl)
{
    qDebug().noquote() << "Installing wireless stack from" << fileUrl << "...";
}

void FlipperZero::installFUS(const QUrl &fileUrl, uint32_t address)
{
    qDebug().noquote().nospace() << "Installing FUS from " << fileUrl << " at the address 0x" << QString::number(address, 16) << "...";
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
    // 1. Device is online and connected in VCP mode
    // 2. There is no ongoing operation

    const auto streamCondition = m_state->isOnline() &&
            !(m_state->isRecoveryMode() || m_state->isPersistent());

    if(streamCondition) {
        m_streamer->start();
    }
}

void FlipperZero::onUpdaterErrorOccured()
{
    auto *instance = qobject_cast<SignalingFailable*>(sender());
    m_state->setErrorString(instance->errorString());
}

void FlipperZero::registerOperation(Zero::AbstractTopLevelOperation *operation)
{
    connect(m_streamer, &ScreenStreamer::stopped, operation, &AbstractOperation::start);
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
        m_streamer->stop();
    }
}

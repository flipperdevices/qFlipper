#include "applicationbackend.h"

#include <QDebug>
#include <QLoggingCategory>

#include "logger.h"
#include "deviceregistry.h"
#include "updateregistry.h"

#include "preferences.h"
#include "flipperupdates.h"

#include "flipperzero/flipperzero.h"
#include "flipperzero/devicestate.h"
#include "flipperzero/assetmanifest.h"
#include "flipperzero/screenstreamer.h"

#include "flipperzero/helper/toplevelhelper.h"

Q_LOGGING_CATEGORY(LOG_BACKEND, "BACKEND")

using namespace Flipper;
using namespace Zero;

ApplicationBackend::ApplicationBackend(QObject *parent):
    QObject(parent),
    m_deviceRegistry(new DeviceRegistry(this)),
    m_firmwareUpdates(new FirmwareUpdates("https://update.flipperzero.one/firmware/directory.json", this)),
    m_state(State::WaitingForDevices),
    m_updateStatus(UpdateStatus::Unknown)
{
    registerMetaTypes();
    registerComparators();

    initConnections();
}

ApplicationBackend::State ApplicationBackend::state() const
{
    return m_state;
}

ApplicationBackend::UpdateStatus ApplicationBackend::updateStatus() const
{
    if(!currentDevice() || !m_firmwareUpdates->isReady()) {
        return UpdateStatus::Unknown;
    }

    const auto &latestVersion = m_firmwareUpdates->latestVersion();

    if (currentDevice()->canRepair(latestVersion)) {
        return UpdateStatus::CanRepair;
    } else if(currentDevice()->canUpdate(latestVersion)) {
        return UpdateStatus::CanUpdate;
    } else if(currentDevice()->canInstall(latestVersion)) {
        return UpdateStatus::CanInstall;
    } else{
        return UpdateStatus::NoUpdates;
    }
}

FlipperZero *ApplicationBackend::currentDevice() const
{
    return m_deviceRegistry->currentDevice();
}

void ApplicationBackend::mainAction()
{
    AbstractOperationHelper *helper;

    if(currentDevice()->deviceState()->isRecoveryMode()) {
        setState(State::RepairingDevice);
        helper = new RepairTopLevelHelper(m_firmwareUpdates, currentDevice(), this);

    } else {
        setState(State::UpdatingDevice);
        helper = new UpdateTopLevelHelper(m_firmwareUpdates, currentDevice(), this);
    }

    connect(helper, &AbstractOperationHelper::finished, helper, &QObject::deleteLater);
}

void ApplicationBackend::createBackup(const QUrl &directoryUrl)
{
    setState(State::CreatingBackup);
    currentDevice()->createBackup(directoryUrl);
}

void ApplicationBackend::restoreBackup(const QUrl &directoryUrl)
{
    setState(State::RestoringBackup);
    currentDevice()->restoreBackup(directoryUrl);
}

void ApplicationBackend::factoryReset()
{
    setState(State::FactoryResetting);
    currentDevice()->factoryReset();
}

void ApplicationBackend::installFirmware(const QUrl &fileUrl)
{
    setState(State::InstallingFirmware);
    currentDevice()->installFirmware(fileUrl);
}

void ApplicationBackend::installWirelessStack(const QUrl &fileUrl)
{
    setState(State::InstallingWirelessStack);
    currentDevice()->installWirelessStack(fileUrl);
}

void ApplicationBackend::installFUS(const QUrl &fileUrl, uint32_t address)
{
    setState(State::InstallingFUS);
    currentDevice()->installFUS(fileUrl, address);
}

void ApplicationBackend::startFullScreenStreaming()
{
    setState(State::ScreenStreaming);
}

void ApplicationBackend::stopFullScreenStreaming()
{
    setState(State::Ready);
}

void ApplicationBackend::finalizeOperation()
{
    qCDebug(LOG_BACKEND) << "Finalized current operation";

    globalLogger->setErrorCount(0);
    m_deviceRegistry->cleanupOffline();

    if(!currentDevice()) {
        setState(State::WaitingForDevices);
        return;

    } else if(currentDevice()->deviceState()->isError()) {
        currentDevice()->restartSession();
    }

    setState(State::Ready);
}

void ApplicationBackend::onCurrentDeviceChanged()
{
    // Should not happen during an ongoing operation
    if(m_state > State::ScreenStreaming && m_state != State::Finished) {
        setState(State::ErrorOccured);
        qCDebug(LOG_BACKEND) << "Current operation was interrupted";

    } else if(currentDevice()) {
        qCDebug(LOG_BACKEND) << "Current device changed to" << currentDevice()->deviceState()->deviceInfo().name;
        // No need to disconnect the old device, as it has been destroyed at this point
        connect(currentDevice(), &FlipperZero::operationFinished, this, &ApplicationBackend::onDeviceOperationFinished);
        connect(currentDevice(), &FlipperZero::stateChanged, this, &ApplicationBackend::updateStatusChanged);
        setState(State::Ready);

    } else {
        qCDebug(LOG_BACKEND) << "Last device was disconnected";
        setState(State::WaitingForDevices);
    }
}

void ApplicationBackend::onDeviceOperationFinished()
{
    // TODO: Some error handling?
    if(!currentDevice()) {
        qCDebug(LOG_BACKEND) << "Lost all connected devices";
        setState(State::ErrorOccured);

    } else if(currentDevice()->deviceState()->isError()) {
        qCDebug(LOG_BACKEND) << "Current operation finished with error:" << currentDevice()->deviceState()->errorString();
        setState(State::ErrorOccured);

    } else {
        setState(State::Finished);
    }
}

void ApplicationBackend::initConnections()
{
    connect(m_deviceRegistry, &DeviceRegistry::currentDeviceChanged, this, &ApplicationBackend::currentDeviceChanged);
    connect(m_deviceRegistry, &DeviceRegistry::currentDeviceChanged, this, &ApplicationBackend::onCurrentDeviceChanged);

    connect(m_deviceRegistry, &DeviceRegistry::currentDeviceChanged, this, &ApplicationBackend::updateStatusChanged);
    connect(m_firmwareUpdates, &UpdateRegistry::latestVersionChanged, this, &ApplicationBackend::updateStatusChanged);
}

void ApplicationBackend::setState(State newState)
{
    if(m_state == newState) {
        return;
    }

    m_state = newState;
    emit stateChanged();
}

UpdateRegistry *ApplicationBackend::firmwareUpdates() const
{
    return m_firmwareUpdates;
}

void ApplicationBackend::registerMetaTypes()
{
    qRegisterMetaType<Preferences*>("Preferences*");
    qRegisterMetaType<Flipper::Updates::FileInfo>("Flipper::Updates::FileInfo");
    qRegisterMetaType<Flipper::Updates::VersionInfo>("Flipper::Updates::VersionInfo");
    qRegisterMetaType<Flipper::Updates::ChannelInfo>("Flipper::Updates::ChannelInfo");

    qRegisterMetaType<Flipper::Zero::DeviceInfo>("Flipper::Zero::DeviceInfo");
    qRegisterMetaType<Flipper::Zero::HardwareInfo>("Flipper::Zero::HardwareInfo");
    qRegisterMetaType<Flipper::Zero::SoftwareInfo>("Flipper::Zero::SoftwareInfo");
    qRegisterMetaType<Flipper::Zero::StorageInfo>("Flipper::Zero::StorageInfo");

    qRegisterMetaType<Flipper::FlipperZero*>("Flipper::FlipperZero*");
    qRegisterMetaType<Flipper::Zero::DeviceState*>("Flipper::Zero::DeviceState*");
    qRegisterMetaType<Flipper::Zero::ScreenStreamer*>("Flipper::Zero::ScreenStreamer*");

    qRegisterMetaType<Flipper::Zero::AssetManifest::FileInfo>();
}

void ApplicationBackend::registerComparators()
{
    QMetaType::registerComparators<Flipper::Zero::AssetManifest::FileInfo>();
}

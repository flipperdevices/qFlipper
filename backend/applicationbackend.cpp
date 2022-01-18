#include "applicationbackend.h"

#include <QDebug>
#include <QLoggingCategory>

#include "logger.h"
#include "deviceregistry.h"
#include "firmwareupdateregistry.h"

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
    m_firmwareUpdateRegistry(new FirmwareUpdateRegistry("https://update.flipperzero.one/firmware/directory.json", this)),
    m_state(State::WaitingForDevices)
{
    registerMetaTypes();
    registerComparators();

    initConnections();
}

ApplicationBackend::State ApplicationBackend::state() const
{
    return m_state;
}

ApplicationBackend::FirmwareUpdateStatus ApplicationBackend::firmwareUpdateStatus() const
{
    if(!device() || !m_firmwareUpdateRegistry->isReady()) {
        return FirmwareUpdateStatus::Unknown;
    }

    const auto &latestVersion = m_firmwareUpdateRegistry->latestVersion();

    if (device()->canRepair(latestVersion)) {
        return FirmwareUpdateStatus::CanRepair;
    } else if(device()->canUpdate(latestVersion)) {
        return FirmwareUpdateStatus::CanUpdate;
    } else if(device()->canInstall(latestVersion)) {
        return FirmwareUpdateStatus::CanInstall;
    } else{
        return FirmwareUpdateStatus::NoUpdates;
    }
}

QAbstractListModel *ApplicationBackend::firmwareUpdateModel() const
{
    return m_firmwareUpdateRegistry;
}

FlipperZero *ApplicationBackend::device() const
{
    return m_deviceRegistry->currentDevice();
}

DeviceState *ApplicationBackend::deviceState() const
{
    if(device()) {
        return device()->deviceState();
    } else {
        return nullptr;
    }
}

const Updates::VersionInfo ApplicationBackend::latestFirmwareVersion() const
{
    return m_firmwareUpdateRegistry->latestVersion();
}

void ApplicationBackend::mainAction()
{
    AbstractOperationHelper *helper;

    if(device()->deviceState()->isRecoveryMode()) {
        setState(State::RepairingDevice);
        helper = new RepairTopLevelHelper(m_firmwareUpdateRegistry, device(), this);

    } else {
        setState(State::UpdatingDevice);
        helper = new UpdateTopLevelHelper(m_firmwareUpdateRegistry, device(), this);
    }

    connect(helper, &AbstractOperationHelper::finished, helper, &QObject::deleteLater);
}

void ApplicationBackend::createBackup(const QUrl &directoryUrl)
{
    setState(State::CreatingBackup);
    device()->createBackup(directoryUrl);
}

void ApplicationBackend::restoreBackup(const QUrl &directoryUrl)
{
    setState(State::RestoringBackup);
    device()->restoreBackup(directoryUrl);
}

void ApplicationBackend::factoryReset()
{
    setState(State::FactoryResetting);
    device()->factoryReset();
}

void ApplicationBackend::installFirmware(const QUrl &fileUrl)
{
    setState(State::InstallingFirmware);
    device()->installFirmware(fileUrl);
}

void ApplicationBackend::installWirelessStack(const QUrl &fileUrl)
{
    setState(State::InstallingWirelessStack);
    device()->installWirelessStack(fileUrl);
}

void ApplicationBackend::installFUS(const QUrl &fileUrl, uint32_t address)
{
    setState(State::InstallingFUS);
    device()->installFUS(fileUrl, address);
}

void ApplicationBackend::startFullScreenStreaming()
{
    setState(State::ScreenStreaming);
}

void ApplicationBackend::stopFullScreenStreaming()
{
    setState(State::Ready);
}

void ApplicationBackend::sendInputEvent(int key, int type)
{
    if(device()) {
        device()->sendInputEvent(key, type);
    }
}

void ApplicationBackend::finalizeOperation()
{
    qCDebug(LOG_BACKEND) << "Finalized current operation";

    globalLogger->setErrorCount(0);
    m_deviceRegistry->removeOfflineDevices();

    if(!device()) {
        setState(State::WaitingForDevices);

    } else {
        device()->finalizeOperation();
        setState(State::Ready);
    }
}

void ApplicationBackend::onCurrentDeviceChanged()
{
    // Should not happen during an ongoing operation
    if(m_state > State::ScreenStreaming && m_state != State::Finished) {
        setState(State::ErrorOccured);
        qCDebug(LOG_BACKEND) << "Current operation was interrupted";

    } else if(device()) {
        qCDebug(LOG_BACKEND) << "Current device changed to" << device()->deviceState()->deviceInfo().name;
        // No need to disconnect the old device, as it has been destroyed at this point
        connect(device(), &FlipperZero::operationFinished, this, &ApplicationBackend::onDeviceOperationFinished);
        connect(device(), &FlipperZero::stateChanged, this, &ApplicationBackend::updateStatusChanged);
        setState(State::Ready);

    } else {
        qCDebug(LOG_BACKEND) << "Last device was disconnected";
        setState(State::WaitingForDevices);
    }
}

void ApplicationBackend::onDeviceOperationFinished()
{
    // TODO: Some error handling?
    if(!device()) {
        qCDebug(LOG_BACKEND) << "Lost all connected devices";
        setState(State::ErrorOccured);

    } else if(device()->deviceState()->isError()) {
        qCDebug(LOG_BACKEND) << "Current operation finished with error:" << device()->deviceState()->errorString();
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
    connect(m_firmwareUpdateRegistry, &UpdateRegistry::latestVersionChanged, this, &ApplicationBackend::updateStatusChanged);
}

void ApplicationBackend::setState(State newState)
{
    if(m_state == newState) {
        return;
    }

    m_state = newState;
    emit stateChanged();
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

    qRegisterMetaType<QAbstractListModel*>();
}

void ApplicationBackend::registerComparators()
{
    QMetaType::registerComparators<Flipper::Zero::AssetManifest::FileInfo>();
}

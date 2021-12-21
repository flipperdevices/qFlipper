#include "applicationbackend.h"

#include <QDebug>
#include <QLoggingCategory>

#include "deviceregistry.h"
#include "updateregistry.h"

#include "preferences.h"
#include "flipperupdates.h"

#include "flipperzero/flipperzero.h"
#include "flipperzero/devicestate.h"
#include "flipperzero/assetmanifest.h"
#include "flipperzero/firmwareupdater.h"
#include "flipperzero/screenstreamer.h"

#include "flipperzero/helper/toplevelhelper.h"

Q_LOGGING_CATEGORY(LOG_BACKEND, "BACKEND")

using namespace Flipper;
using namespace Zero;

// BIG TODO: Abstract away all mentions on Flipper Zero internals into common API
// (to facilitate other devices support)

ApplicationBackend::ApplicationBackend(QObject *parent):
    QObject(parent),
    m_deviceRegistry(new DeviceRegistry(this)),
    m_firmwareUpdates(new FirmwareUpdates("https://update.flipperzero.one/firmware/directory.json", this)),
    m_applicationUpdates(new ApplicationUpdates("https://update.flipperzero.one/qFlipper/directory.json", this)),
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

    connect(helper, &AbstractOperationHelper::finished, this, [=]() {
        if(helper->isError()) {
            qCCritical(LOG_BACKEND).noquote() << "Failed to complete the operation:" << helper->errorString();
        }

        helper->deleteLater();
    });
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

void ApplicationBackend::onCurrentDeviceChanged()
{
    // Should not happen during an ongoing operation
    if(m_state != State::Ready && m_state != State::WaitingForDevices &&
       m_state != State::Finished) {
        setState(State::OperationInterrupted);
        qCCritical(LOG_BACKEND) << "Current operation was interrupted";

    } else if(m_deviceRegistry->currentDevice()) {
        // No need to disconnect the old device, as it has been destroyed at this point
        connect(currentDevice(), &FlipperZero::operationFinished, this, &ApplicationBackend::onDeviceOperationFinished);
        setState(State::Ready);

    } else {
        setState(State::WaitingForDevices);
    }
}

void ApplicationBackend::onDeviceOperationFinished()
{
    // TODO: Some error handling?
    setState(State::Ready);
}

void ApplicationBackend::initConnections()
{
    connect(m_deviceRegistry, &DeviceRegistry::currentDeviceChanged, this, &ApplicationBackend::currentDeviceChanged);
    connect(m_deviceRegistry, &DeviceRegistry::currentDeviceChanged, this, &ApplicationBackend::onCurrentDeviceChanged);
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

UpdateRegistry *ApplicationBackend::applicationUpdates() const
{
    return m_applicationUpdates;
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
    qRegisterMetaType<Flipper::Zero::FirmwareUpdater*>("Flipper::Zero::FirmwareUpdater*");
    qRegisterMetaType<Flipper::Zero::ScreenStreamer*>("Flipper::Zero::ScreenStreamer*");

    qRegisterMetaType<Flipper::Zero::AssetManifest::FileInfo>();
}

void ApplicationBackend::registerComparators()
{
    QMetaType::registerComparators<Flipper::Zero::AssetManifest::FileInfo>();
}

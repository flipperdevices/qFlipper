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

void ApplicationBackend::mainAction()
{
    setState(State::UpdatingDevice);

    auto *helper = new Zero::UpdateTopLevelHelper(m_firmwareUpdates, m_deviceRegistry->currentDevice(), this);

    connect(helper, &AbstractOperationHelper::finished, this, [=]() {
        if(helper->isError()) {
            qCCritical(LOG_BACKEND).noquote() << "Failed to complete the operation:" << helper->errorString();
        } else {
            setState(State::Ready);
        }

        helper->deleteLater();
    });
}

void ApplicationBackend::createBackup(const QUrl &directoryUrl)
{
    qCDebug(LOG_BACKEND).noquote() << "Creating backup in" << directoryUrl << "...";
}

void ApplicationBackend::restoreBackup(const QUrl &directoryUrl)
{
    qCDebug(LOG_BACKEND).noquote() << "Restoring backup from" << directoryUrl << "...";
}

void ApplicationBackend::factoryReset()
{
    qCDebug(LOG_BACKEND) << "Executing factory reset...";
}

void ApplicationBackend::installFirmware(const QUrl &fileUrl)
{
    qCDebug(LOG_BACKEND).noquote() << "Installing firmware from" << fileUrl << "...";
}

void ApplicationBackend::installWirelessStack(const QUrl &fileUrl)
{
    qCDebug(LOG_BACKEND).noquote() << "Installing wireless stack from" << fileUrl << "...";
}

void ApplicationBackend::installFUS(const QUrl &fileUrl, uint32_t address)
{
    qCDebug(LOG_BACKEND).noquote().nospace() << "Installing FUS from " << fileUrl << " at the address 0x" << QString::number(address, 16) << "...";
}

void ApplicationBackend::onDevicesChanged()
{
    if(m_deviceRegistry->currentDevice()) {
        setState(m_state == State::WaitingForDevices ? State::Ready : m_state);
    } else {
        // TODO: Doesn't cover all of the cases, add currentDeviceChanged() signal to DeviceRegistry
        setState(m_state == State::Ready ? State::WaitingForDevices : State::OperationInterrupted);
    }

    qCDebug(LOG_BACKEND) << "State changed, current state:" << m_state;
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

void ApplicationBackend::initConnections()
{
    connect(m_deviceRegistry, &DeviceRegistry::devicesChanged, this, &ApplicationBackend::onDevicesChanged);
}

void ApplicationBackend::setState(State newState)
{
    if(m_state == newState) {
        return;
    }

    m_state = newState;
    emit stateChanged();
}

DeviceRegistry *ApplicationBackend::deviceRegistry() const
{
    return m_deviceRegistry;
}

UpdateRegistry *ApplicationBackend::firmwareUpdates() const
{
    return m_firmwareUpdates;
}

UpdateRegistry *ApplicationBackend::applicationUpdates() const
{
    return m_applicationUpdates;
}

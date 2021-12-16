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

Q_LOGGING_CATEGORY(LOG_BACKEND, "BACKEND")

using namespace Flipper;

ApplicationBackend::ApplicationBackend(QObject *parent):
    QObject(parent),
    m_deviceRegistry(new DeviceRegistry(this)),
    m_firmwareUpdates(new FirmwareUpdates("https://update.flipperzero.one/firmware/directory.json", this)),
    m_applicationUpdates(new ApplicationUpdates("https://update.flipperzero.one/qFlipper/directory.json", this))
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
    QMetaType::registerComparators<Flipper::Zero::AssetManifest::FileInfo>();
}

void ApplicationBackend::mainAction()
{
    qCInfo(LOG_BACKEND) << "Performing main action...";
}

void ApplicationBackend::createBackup()
{
    qCInfo(LOG_BACKEND) << "Creating backup...";
}

void ApplicationBackend::restoreBackup()
{
    qCInfo(LOG_BACKEND) << "Restoring backup...";
}

void ApplicationBackend::factoryReset()
{
    qCInfo(LOG_BACKEND) << "Executing factory reset...";
}

void ApplicationBackend::installFirmware(const QUrl &fileUrl)
{
    qCInfo(LOG_BACKEND).noquote() << "Installing firmware from" << fileUrl << "...";
}

void ApplicationBackend::installWirelessStack(const QUrl &fileUrl)
{
    qCInfo(LOG_BACKEND).noquote() << "Installing wireless stack from" << fileUrl << "...";
}

void ApplicationBackend::installFUS(const QUrl &fileUrl, uint32_t address)
{
    qCInfo(LOG_BACKEND).noquote().nospace() << "Installing FUS from " << fileUrl << " at the address 0x" << QString::number(address, 16) << "...";
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

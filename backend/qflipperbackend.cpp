#include "qflipperbackend.h"

#include "preferences.h"
#include "flipperupdates.h"

#include "flipperzero/flipperzero.h"
#include "flipperzero/devicestate.h"
#include "flipperzero/assetmanifest.h"
#include "flipperzero/firmwareupdater.h"
#include "flipperzero/screenstreamer.h"

using namespace Flipper;

QFlipperBackend::QFlipperBackend(QObject *parent):
    QObject(parent),
    firmwareUpdates("https://update.flipperzero.one/firmware/directory.json"),
    applicationUpdates("https://update.flipperzero.one/qFlipper/directory.json")
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

QFlipperBackend::~QFlipperBackend()
{}

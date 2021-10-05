#include "qflipperbackend.h"

#include "flipperupdates.h"

#include "flipperzero/flipperzero.h"
#include "flipperzero/devicestate.h"
#include "flipperzero/assetmanifest.h"
#include "flipperzero/firmwareupdater.h"
#include "flipperzero/screenstreaminterface.h"

#include "macros.h"

using namespace Flipper;

QFlipperBackend::QFlipperBackend()
{
    qRegisterMetaType<Flipper::Updates::FileInfo>("Flipper::Updates::FileInfo");
    qRegisterMetaType<Flipper::Updates::VersionInfo>("Flipper::Updates::VersionInfo");
    qRegisterMetaType<Flipper::Updates::ChannelInfo>("Flipper::Updates::ChannelInfo");

    qRegisterMetaType<Flipper::Zero::DeviceState*>("Flipper::Zero::DeviceState*");
    qRegisterMetaType<Flipper::Zero::FirmwareUpdater*>("Flipper::Zero::FirmwareUpdater*");
    qRegisterMetaType<Flipper::Zero::ScreenStreamInterface*>("Flipper::Zero::ScreenStreamInterface*");

    qRegisterMetaType<Flipper::Zero::AssetManifest::FileInfo>();
    QMetaType::registerComparators<Flipper::Zero::AssetManifest::FileInfo>();
}

QFlipperBackend::~QFlipperBackend()
{}

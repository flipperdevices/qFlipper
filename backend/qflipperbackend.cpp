#include "qflipperbackend.h"

#include "flipperzero/flipperzero.h"
#include "flipperzero/assetmanifest.h"
#include "flipperzero/screenstreaminterface.h"
#include "flipperzero/recoveryinterface.h"

#include "macros.h"

using namespace Flipper;

QFlipperBackend::QFlipperBackend():
    firmwareUpdates("https://update.flipperzero.one/firmware/directory.json"),
    applicationUpdates("https://update.flipperzero.one/qFlipper/directory.json")
{
    qRegisterMetaType<Flipper::Updates::FileInfo>("Flipper::Updates::FileInfo");
    qRegisterMetaType<Flipper::Updates::VersionInfo>("Flipper::Updates::VersionInfo");
    qRegisterMetaType<Flipper::Updates::ChannelInfo>("Flipper::Updates::ChannelInfo");
    qRegisterMetaType<Flipper::Zero::ScreenStreamInterface*>("Flipper::Zero::RemoteController*");
    qRegisterMetaType<Flipper::Zero::RecoveryInterface*>("Flipper::Zero::RecoveryInterface*");

    qRegisterMetaType<Flipper::Zero::AssetManifest::FileInfo>();
    QMetaType::registerComparators<Flipper::Zero::AssetManifest::FileInfo>();
}

QFlipperBackend::~QFlipperBackend()
{}

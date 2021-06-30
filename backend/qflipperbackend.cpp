#include "qflipperbackend.h"

#include "flipperzero.h"
#include "flipperzeroremote.h"

using namespace Flipper;

QFlipperBackend::QFlipperBackend()
{
    qRegisterMetaType<Updates::FileInfo>("Updates::FileInfo");
    qRegisterMetaType<Flipper::ZeroRemote*>("Flipper::ZeroRemote*");

    QObject::connect(&deviceRegistry, &DeviceRegistry::deviceConnected, &downloader, &FirmwareDownloader::onDeviceConnected);
}

QFlipperBackend::~QFlipperBackend()
{}

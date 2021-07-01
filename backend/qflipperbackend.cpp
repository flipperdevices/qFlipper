#include "qflipperbackend.h"

#include "flipperzero/flipperzero.h"
#include "flipperzero/remotecontroller.h"

using namespace Flipper;

QFlipperBackend::QFlipperBackend()
{
    qRegisterMetaType<Updates::FileInfo>("Updates::FileInfo");
    qRegisterMetaType<Flipper::Zero::RemoteController*>("Flipper::Zero::RemoteController*");

    QObject::connect(&deviceRegistry, &DeviceRegistry::deviceConnected, &downloader, &FirmwareDownloader::onDeviceConnected);
}

QFlipperBackend::~QFlipperBackend()
{}

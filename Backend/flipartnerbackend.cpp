#include "flipartnerbackend.h"

#include "flipperzero.h"
#include "flipperzeroremote.h"

using namespace Flipper;

FlipartnerBackend::FlipartnerBackend()
{
    qRegisterMetaType<Updates::FileInfo>("Updates::FileInfo");
    qRegisterMetaType<Flipper::ZeroRemote*>("Flipper::ZeroRemote*");

    QObject::connect(&deviceRegistry, &DeviceRegistry::deviceConnected, &downloader, &FirmwareDownloader::onDeviceConnected);
}

FlipartnerBackend::~FlipartnerBackend()
{}

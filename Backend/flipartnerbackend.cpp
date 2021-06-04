#include "flipartnerbackend.h"

#include "flipperzero.h"

using namespace Flipper;

FlipartnerBackend::FlipartnerBackend()
{
    qRegisterMetaType<Updates::FileInfo>("Updates::FileInfo");

    QObject::connect(&deviceRegistry, &DeviceRegistry::deviceConnected, &downloader, &FirmwareDownloader::onDeviceConnected);
}

FlipartnerBackend::~FlipartnerBackend()
{}

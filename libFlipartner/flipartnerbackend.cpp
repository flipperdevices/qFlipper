#include "flipartnerbackend.h"

#include "flipperzero.h"

using namespace Flipper;

FlipartnerBackend::FlipartnerBackend()
{
    QObject::connect(&deviceRegistry, &DeviceRegistry::deviceConnected, &downloader, &FirmwareDownloader::onDeviceConnected);
}

FlipartnerBackend::~FlipartnerBackend()
{}

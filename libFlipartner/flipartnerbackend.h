#ifndef FLIPARTNERBACKEND_H
#define FLIPARTNERBACKEND_H

#include "deviceregistry.h"
#include "updateregistry.h"
#include "firmwaredownloader.h"

struct FlipartnerBackend
{
    FlipartnerBackend();
    ~FlipartnerBackend();

    Flipper::DeviceRegistry deviceRegistry;
    Flipper::UpdateRegistry updateRegistry;
    Flipper::FirmwareDownloader downloader;
};

#endif // FLIPARTNERBACKEND_H

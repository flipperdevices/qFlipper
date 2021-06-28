#ifndef QFLIPPERBACKEND_H
#define QFLIPPERBACKEND_H

#include "deviceregistry.h"
#include "updateregistry.h"
#include "firmwaredownloader.h"

struct QFlipperBackend
{
    QFlipperBackend();
    ~QFlipperBackend();

    Flipper::DeviceRegistry deviceRegistry;
    Flipper::UpdateRegistry updateRegistry;
    Flipper::FirmwareDownloader downloader;
};

#endif // QFLIPPERBACKEND_H

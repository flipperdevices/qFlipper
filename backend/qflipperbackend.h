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
    Flipper::UpdateRegistry firmwareUpdates;
    Flipper::UpdateRegistry applicationUpdates;
    Flipper::FirmwareDownloader downloader;
};

#endif // QFLIPPERBACKEND_H

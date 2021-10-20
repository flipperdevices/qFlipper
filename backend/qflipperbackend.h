#pragma once

#include "deviceregistry.h"
#include "updateregistry.h"

struct QFlipperBackend
{
    QFlipperBackend();
    ~QFlipperBackend();

    Flipper::DeviceRegistry deviceRegistry;
    Flipper::FirmwareUpdates firmwareUpdates;
    Flipper::ApplicationUpdates applicationUpdates;
};

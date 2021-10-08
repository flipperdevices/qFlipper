#ifndef QFLIPPERBACKEND_H
#define QFLIPPERBACKEND_H

#include "deviceregistry.h"
#include "updateregistry.h"

struct QFlipperBackend
{
    QFlipperBackend();
    ~QFlipperBackend();

    Flipper::DeviceRegistry deviceRegistry;
    Flipper::UpdateRegistry firmwareUpdates;
    Flipper::UpdateRegistry applicationUpdates;
};

#endif // QFLIPPERBACKEND_H

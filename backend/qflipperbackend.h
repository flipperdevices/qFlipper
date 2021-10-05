#ifndef QFLIPPERBACKEND_H
#define QFLIPPERBACKEND_H

#include "deviceregistry.h"

struct QFlipperBackend
{
    QFlipperBackend();
    ~QFlipperBackend();

    Flipper::DeviceRegistry deviceRegistry;
};

#endif // QFLIPPERBACKEND_H

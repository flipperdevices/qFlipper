#ifndef FLIPARTNERBACKEND_H
#define FLIPARTNERBACKEND_H

#include "flipperdetector.h"
#include "firmwareupdater.h"

struct FlipartnerBackend
{
    FlipartnerBackend();
    ~FlipartnerBackend();

    FlipperDetector detector;
    FirmwareUpdater updater;
};

#endif // FLIPARTNERBACKEND_H

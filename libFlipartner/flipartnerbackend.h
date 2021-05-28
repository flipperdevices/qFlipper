#ifndef FLIPARTNERBACKEND_H
#define FLIPARTNERBACKEND_H

#include "updateslistmodel.h"
#include "flipperlistmodel.h"
#include "flipperdetector.h"
#include "firmwareupdater.h"

struct FlipartnerBackend
{
    FlipartnerBackend();
    ~FlipartnerBackend();

    FlipperListModel mainList;
    FlipperDetector detector;
    FirmwareUpdater updater;
    UpdatesListModel updates;
};

#endif // FLIPARTNERBACKEND_H

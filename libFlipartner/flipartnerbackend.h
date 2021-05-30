#ifndef FLIPARTNERBACKEND_H
#define FLIPARTNERBACKEND_H

//#include "updateslistmodel.h"
//#include "flipperlistmodel.h"
//#include "flipperdetector.h"
//#include "firmwareupdater.h"

#include "registry.h"

struct FlipartnerBackend
{
    FlipartnerBackend();
    ~FlipartnerBackend();

    Flipper::Registry registry;
//    FlipperListModel mainList;
//    FlipperDetector detector;
//    FirmwareUpdater updater;
//    UpdatesListModel updates;
};

#endif // FLIPARTNERBACKEND_H

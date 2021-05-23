#include "flipartnerbackend.h"

FlipartnerBackend::FlipartnerBackend()
{
    QObject::connect(&detector, &FlipperDetector::flipperDetected, &mainList, &FlipperListModel::insertDevice);
    QObject::connect(&detector, &FlipperDetector::flipperDisconnected, &mainList, &FlipperListModel::removeDevice);
    QObject::connect(&updater, &FirmwareUpdater::deviceInfoRequested, &mainList, &FlipperListModel::requestDevice);

    QObject::connect(&mainList, &FlipperListModel::deviceFound, &updater, &FirmwareUpdater::onDeviceFound);
    QObject::connect(&detector, &FlipperDetector::flipperDetected, &updater, &FirmwareUpdater::onDeviceConnected);
}

FlipartnerBackend::~FlipartnerBackend()
{}

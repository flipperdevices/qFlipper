#include "flipartnerbackend.h"

FlipartnerBackend::FlipartnerBackend()
{
    qRegisterMetaType<FlipperInfo>("FlipperInfo");

    QObject::connect(&detector, &FlipperDetector::flipperDetected, &mainList, &FlipperListModel::insertDevice);
    QObject::connect(&detector, &FlipperDetector::flipperDisconnected, &mainList, &FlipperListModel::removeDevice);
    QObject::connect(&detector, &FlipperDetector::flipperUpdated, &mainList, &FlipperListModel::updateDevice);

    QObject::connect(&updater, &FirmwareUpdater::deviceInfoRequested, &mainList, &FlipperListModel::requestDevice);

    QObject::connect(&mainList, &FlipperListModel::deviceFound, &updater, &FirmwareUpdater::onDeviceFound);
    QObject::connect(&detector, &FlipperDetector::flipperDetected, &updater, &FirmwareUpdater::onDeviceConnected);
}

FlipartnerBackend::~FlipartnerBackend()
{}

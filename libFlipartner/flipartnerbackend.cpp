#include "flipartnerbackend.h"

FlipartnerBackend::FlipartnerBackend()
{
    qRegisterMetaType<FlipperInfo>("FlipperInfo");

    QObject::connect(&detector, &FlipperDetector::flipperDetected, &mainList, &FlipperListModel::insertDevice);
    QObject::connect(&detector, &FlipperDetector::flipperDisconnected, &mainList, &FlipperListModel::removeDevice);
    QObject::connect(&detector, &FlipperDetector::flipperUpdated, &mainList, &FlipperListModel::updateDevice);

    QObject::connect(&updater, &FirmwareUpdater::deviceStatusChanged, &mainList, &FlipperListModel::updateDeviceStatus);

    QObject::connect(&detector, &FlipperDetector::flipperDetected, &updater, &FirmwareUpdater::onDeviceConnected);
}

FlipartnerBackend::~FlipartnerBackend()
{}

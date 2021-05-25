#include "flipperdetector.h"

#include <QDebug>
#include <QThreadPool>

#include "usbdeviceparams.h"
#include "usbdevice.h"

#include "flipperinfotask.h"

FlipperDetector::FlipperDetector(QObject *parent):
    QObject(parent)
{
    // Queued connection in order to isolate from backend callback context - dirty!
    connect(&USBDevice::backend(), &USBBackend::devicePluggedIn, this, &FlipperDetector::onDevicePluggedIn, Qt::QueuedConnection);
    connect(&USBDevice::backend(), &USBBackend::deviceUnplugged, this, &FlipperDetector::onDeviceUnplugged, Qt::QueuedConnection);

    const USBBackend::DeviceList params = {
        {
            0x0483,
            0xdf11,
            "",
            "",
            "",
            nullptr
        },

        {
            0x0483,
            0x5740,
            "Flipper Devices Inc.",
            "Flipper Control Virtual ComPort",
            "",
            nullptr
        }
    };

    USBDevice::backend().registerHotplugEvent(params);
}

void FlipperDetector::onDevicePluggedIn(USBDeviceParams params)
{
    // TODO: Error handling
    USBBackend::getExtraDeviceInfo(params);

    const FlipperInfo info(params);
    emit flipperDetected(info);

    auto *infoTask = new FlipperInfoTask(info);
    connect(infoTask, &FlipperInfoTask::finished, this, &FlipperDetector::flipperUpdated);
    QThreadPool::globalInstance()->start(infoTask);
}

void FlipperDetector::onDeviceUnplugged(USBDeviceParams params)
{
    emit flipperDisconnected(params);
}

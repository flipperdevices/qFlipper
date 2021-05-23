#include "flipperdetector.h"

#include <QDebug>
#include <QThread>
#include <QtConcurrent/QtConcurrentRun>

#include "usbdeviceparams.h"
#include "usbdevice.h"

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
    USBBackend::getExtraDeviceInfo(params);

    emit flipperDetected({
        "Flipper Zero",
        "N@m3_42",
        "N/A",
        params
    });
}

void FlipperDetector::onDeviceUnplugged(USBDeviceParams params)
{
    emit flipperDisconnected({
        "", "", "",
        params
    });
}

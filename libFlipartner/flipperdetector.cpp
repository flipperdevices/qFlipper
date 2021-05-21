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

const QList<FlipperInfo> FlipperDetector::devices()
{
    QList<FlipperInfo> list = {
//        {
//            "Flipper Zero",
//            "Ch1rp78",
//            "0.15.0-1",
//            "34095235DF03W35",
//            {}
//        },
        {
            "Flipper Zero",
            "8uzz3r66",
            "0.16.0-3",
            "0234GF234QD342",
            {}
        },
    };

    return list + m_infos;
}

void FlipperDetector::onDevicePluggedIn(USBDeviceParams params)
{
    USBBackend::getExtraDeviceInfo(params);

    FlipperInfo newInfo = {
        "Flipper Zero",
        "N/A",
        "N/A",
        params.serialNumber,
        params
    };

    m_infos.append(newInfo);
    emit devicesChanged(m_infos);
}

void FlipperDetector::onDeviceUnplugged(USBDeviceParams params)
{
    for(auto it = m_infos.begin(); it != m_infos.end(); ++it) {
        if((*it).params.uniqueID == params.uniqueID) {
            m_infos.erase(it);
        }
    }

    emit devicesChanged(m_infos);
}

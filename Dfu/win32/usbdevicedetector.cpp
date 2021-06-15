#include "usbdevicedetector.h"

#include <QTimer>
#include <QThread>

#include "usbdevicedetector_p.h"
#include "macros.h"

USBDeviceDetector::USBDeviceDetector(QObject *parent):
    QObject(parent),
    m_worker(nullptr)
{}

USBDeviceDetector::~USBDeviceDetector()
{}

USBDeviceDetector *USBDeviceDetector::instance()
{
    static USBDeviceDetector instance;
    return &instance;
}

bool USBDeviceDetector::setWantedDevices(const QList<USBDeviceInfo> &wantedList)
{
    if(m_worker) {
        disconnect(m_worker, &USBDeviceDetectorWorker::devicePluggedIn, this, &USBDeviceDetector::devicePluggedIn);
        disconnect(m_worker, &USBDeviceDetectorWorker::deviceUnplugged, this, &USBDeviceDetector::deviceUnplugged);

        m_worker->deleteLater();
    }

    m_worker = new USBDeviceDetectorWorker(wantedList, this);
    connect(m_worker, &USBDeviceDetectorWorker::devicePluggedIn, this, &USBDeviceDetector::devicePluggedIn);
    connect(m_worker, &USBDeviceDetectorWorker::deviceUnplugged, this, &USBDeviceDetector::deviceUnplugged);

    return true;
}

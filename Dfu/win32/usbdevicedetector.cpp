#include "usbdevicedetector.h"

#include <QTimer>
#include <QTimerEvent>

#include "macros.h"

USBDeviceDetector::USBDeviceDetector(QObject *parent):
    QObject(parent)
{
}

USBDeviceDetector::~USBDeviceDetector()
{
}

USBDeviceDetector *USBDeviceDetector::instance()
{
    static USBDeviceDetector instance;
    return &instance;
}

bool USBDeviceDetector::setWantedDevices(const QList<USBDeviceInfo> &wantedList)
{
}

void USBDeviceDetector::timerEvent(QTimerEvent *e)
{
    e->accept();
}

static USBDeviceInfo getDeviceInfo(const USBDeviceInfo &info)
{
}

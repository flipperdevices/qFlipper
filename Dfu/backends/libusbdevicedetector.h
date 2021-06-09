#ifndef USBDEVICEDETECTOR_H
#define USBDEVICEDETECTOR_H

#include <QObject>

#include "usbdeviceparams.h"

class DeviceWatcher;
class USBDeviceDetector : public QObject
{
    Q_OBJECT

public:
    USBDeviceDetector(QObject *parent = nullptr);
    bool setWantedDevices(const QList <USBDeviceParams> &wantedList);

signals:
    void devicePluggedIn(const USBDeviceParams&);
    void deviceUnplugged(const USBDeviceParams&);

private:
    void timerEvent(QTimerEvent *e) override;
    DeviceWatcher *m_watcher;
};

#endif // USBDEVICEDETECTOR_H

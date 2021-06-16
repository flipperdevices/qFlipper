#ifndef USBDEVICEDETECTORPRIVATE_H
#define USBDEVICEDETECTORPRIVATE_H

#include <QObject>

#include "usbdeviceinfo.h"

class USBDeviceDetectorWorker : public QObject
{
    Q_OBJECT

public:
    USBDeviceDetectorWorker(const QList<USBDeviceInfo> &wanted, QObject *parent = nullptr);

signals:
    void devicePluggedIn(const USBDeviceInfo&);
    void deviceUnplugged(const USBDeviceInfo&);

private:
    // TODO: read windows hotplug events
    void timerEvent(QTimerEvent *e) override;

    QList <USBDeviceInfo> availableDevices() const;

    void processDevicesArrived(const QList <USBDeviceInfo> &available);
    void processDevicesLeft(const QList <USBDeviceInfo> &available);

    static USBDeviceInfo parseInstanceID(const char *buf);

    QList <USBDeviceInfo> m_wanted;
    QList <USBDeviceInfo> m_current;
};

#endif // USBDEVICEDETECTORPRIVATE_H

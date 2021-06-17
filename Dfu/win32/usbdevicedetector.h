#ifndef WIN32DEVICEDETECTOR_H
#define WIN32DEVICEDETECTOR_H

#include <QObject>

#include "usbdeviceinfo.h"

class USBDeviceDetectorWorker;

class USBDeviceDetector : public QObject
{
    Q_OBJECT

    USBDeviceDetector(QObject *parent = nullptr);
    ~USBDeviceDetector();

public:
    static USBDeviceDetector *instance();

    bool setWantedDevices(const QList <USBDeviceInfo> &wantedList);

signals:
    void devicePluggedIn(const USBDeviceInfo&);
    void deviceUnplugged(const USBDeviceInfo&);

private:
    USBDeviceDetectorWorker *m_worker;
};

#endif // WIN32DEVICEDETECTOR_H
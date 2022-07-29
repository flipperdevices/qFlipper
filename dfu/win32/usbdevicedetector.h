#pragma once

#include <QObject>

#include "usbdeviceinfo.h"

class USBDeviceDetector : public QObject
{
    Q_OBJECT

    struct USBDeviceDetectorPrivate;

public:
    USBDeviceDetector(QObject *parent = nullptr);
    ~USBDeviceDetector();

    void setLogLevel(int logLevel);
    bool setWantedDevices(const QList <USBDeviceInfo> &wantedList);

public slots:
    void update();

signals:
    void devicePluggedIn(const USBDeviceInfo&);
    void deviceUnplugged(const USBDeviceInfo&);

private:
    bool registerAtom();
    bool createHotplugWindow();

    QList <USBDeviceInfo> availableDevices() const;

    void processDevicesArrived(const QList <USBDeviceInfo> &available);
    void processDevicesLeft(const QList <USBDeviceInfo> &available);

    static USBDeviceInfo parseInstanceID(const char *buf);

    USBDeviceDetectorPrivate *m_p;
    QList <USBDeviceInfo> m_wanted;
    QList <USBDeviceInfo> m_current;
};

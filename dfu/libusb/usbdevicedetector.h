#pragma once

#include <QObject>

#include "usbdeviceinfo.h"

class USBDeviceDetector : public QObject
{
    Q_OBJECT

public:
    USBDeviceDetector(QObject *parent = nullptr);
    ~USBDeviceDetector();

    bool setWantedDevices(const QList <USBDeviceInfo> &wantedList);

signals:
    void devicePluggedIn(const USBDeviceInfo&);
    void deviceUnplugged(const USBDeviceInfo&);

private:
    void timerEvent(QTimerEvent *e) override;
};

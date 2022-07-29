#pragma once

#include <QVector>
#include <QObject>

#include "usbdeviceinfo.h"

class QTimer;

class USBDeviceDetector : public QObject
{
    Q_OBJECT

public:
    USBDeviceDetector(QObject *parent = nullptr);
    ~USBDeviceDetector();

    void setLogLevel(int logLevel);
    bool setWantedDevices(const QList <USBDeviceInfo> &wantedList);

    void registerDevice(const USBDeviceInfo &deviceInfo);
    void unregisterDevice(const USBDeviceInfo &deviceInfo);

signals:
    void devicePluggedIn(const USBDeviceInfo&);
    void deviceUnplugged(const USBDeviceInfo&);

private slots:
    void processEvents();

private:
    static USBDeviceInfo fillDeviceInfo(const USBDeviceInfo &deviceInfo);

    QTimer *m_timer;
    QVector<USBDeviceInfo> m_devices;
};

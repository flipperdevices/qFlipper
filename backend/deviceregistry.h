#pragma once

#include <QObject>
#include <QVector>

#include "usbdeviceinfo.h"

namespace Flipper {

class FlipperZero;

class DeviceRegistry : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Flipper::FlipperZero* currentDevice READ currentDevice NOTIFY devicesChanged)

    using DeviceList = QVector<FlipperZero*>;

public:
    DeviceRegistry(QObject *parent = nullptr);

    FlipperZero *currentDevice() const;

signals:
    void deviceConnected(FlipperZero*);
    void devicesChanged();

public slots:
    void insertDevice(const USBDeviceInfo &info);
    void removeDevice(const USBDeviceInfo &info);

private slots:
    void processDevice();

private:
    DeviceList m_devices;
};

}

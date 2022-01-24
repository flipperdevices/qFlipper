#pragma once

#include <QObject>
#include <QVector>

#include "usbdeviceinfo.h"

namespace Flipper {

class FlipperZero;

class DeviceRegistry : public QObject
{
    Q_OBJECT

    using DeviceList = QVector<FlipperZero*>;

public:
    enum DeviceRegistryError {
        NoError,
        InvalidDevice,
        SerialError,
        RecoveryError
    };

    DeviceRegistry(QObject *parent = nullptr);

    FlipperZero *currentDevice() const;
    int deviceCount() const;

    DeviceRegistryError error() const;
    void clearError();

    bool isQueryInProgress() const;

signals:
    void isQueryInProgressChanged();
    void currentDeviceChanged();
    void deviceCountChanged();
    void errorChanged();

public slots:
    void insertDevice(const USBDeviceInfo &info);
    void removeDevice(const USBDeviceInfo &info);
    void removeOfflineDevices();

private slots:
    void processDevice();

private:
    void setError(DeviceRegistryError newError);
    void setQueryInProgress(bool set);

    DeviceList m_devices;
    DeviceRegistryError m_error;
    bool m_isQueryInProgress;
};

}

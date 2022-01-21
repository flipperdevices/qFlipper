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
    enum class Error {
        NoError,
        InvalidDevice,
        PortAccessError,
        USBAccessError,
        ProtocolError
    };

    DeviceRegistry(QObject *parent = nullptr);

    FlipperZero *currentDevice() const;
    int deviceCount() const;

    Error error() const;
    void clearError();

signals:
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
    void setError(Error newError);

    DeviceList m_devices;
    Error m_error;
};

}

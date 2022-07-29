#pragma once

#include <QObject>
#include <QVector>

#include "backenderror.h"
#include "usbdeviceinfo.h"

class USBDeviceDetector;

namespace Flipper {

class FlipperZero;

class DeviceRegistry : public QObject
{
    Q_OBJECT

    using DeviceList = QVector<FlipperZero*>;

public:
    DeviceRegistry(QObject *parent = nullptr);

    void setBackendLogLevel(int logLevel);

    FlipperZero *currentDevice() const;
    int deviceCount() const;

    BackendError::ErrorType error() const;
    void clearError();

    bool isQueryInProgress() const;

signals:
    void isQueryInProgressChanged();
    void currentDeviceChanged();
    void deviceCountChanged();
    void errorOccured();

public slots:
    void insertDevice(const USBDeviceInfo &info);
    void removeDevice(const USBDeviceInfo &info);
    void removeOfflineDevices();

private slots:
    void processDevice();

private:
    void setError(BackendError::ErrorType newError);
    void setQueryInProgress(bool set);

    USBDeviceDetector *m_detector;
    DeviceList m_devices;
    BackendError::ErrorType m_error;
    bool m_isQueryInProgress;
};

}

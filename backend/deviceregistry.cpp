#include "deviceregistry.h"

#include <QDebug>
#include <QMetaObject>
#include <QLoggingCategory>

#include "flipperzero/helper/deviceinfohelper.h"
#include "flipperzero/flipperzero.h"
#include "flipperzero/devicestate.h"

#include "usbdevice.h"

#define FLIPPER_ZERO_VID 0x0483
#define FLIPPER_ZERO_PID_VCP 0x5740
#define FLIPPER_ZERO_PID_DFU 0xdf11

Q_LOGGING_CATEGORY(LOG_DEVREG, "REG");

using namespace Flipper;

DeviceRegistry::DeviceRegistry(QObject *parent):
    QObject(parent),
    m_detector(new USBDeviceDetector(this)),
    m_error(BackendError::UnknownError),
    m_isQueryInProgress(false)
{
    connect(m_detector, &USBDeviceDetector::devicePluggedIn, this, &DeviceRegistry::insertDevice);
    connect(m_detector, &USBDeviceDetector::deviceUnplugged, this, &DeviceRegistry::removeDevice);

    m_detector->setWantedDevices({
        USBDeviceInfo(FLIPPER_ZERO_VID, FLIPPER_ZERO_PID_DFU),
        USBDeviceInfo(FLIPPER_ZERO_VID, FLIPPER_ZERO_PID_VCP)
            .withManufacturer("Flipper Devices Inc.")
            .withProductDescription("Flipper Control Virtual ComPort")
                                 });
}

void DeviceRegistry::setBackendLogLevel(int logLevel)
{
    m_detector->setLogLevel(logLevel);
}

FlipperZero *DeviceRegistry::currentDevice() const
{
    return m_devices.isEmpty() ? nullptr : m_devices.first();
}

int DeviceRegistry::deviceCount() const
{
    return m_devices.size();
}

BackendError::ErrorType DeviceRegistry::error() const
{
    return m_error;
}

void DeviceRegistry::clearError()
{
    setError(BackendError::UnknownError);
}

bool DeviceRegistry::isQueryInProgress() const
{
    return m_isQueryInProgress;
}

void DeviceRegistry::insertDevice(const USBDeviceInfo &info)
{
    if(!info.isComplete()) {
        qCDebug(LOG_DEVREG).noquote().nospace()
            << "Incomplete device info: VID_0x" << QString::number(info.vendorID(), 16) << ":PID_0x"
            << QString::number(info.productID(), 16);

        setError(BackendError::InvalidDevice);

    } else if(info.vendorID() != FLIPPER_ZERO_VID) {
        qCDebug(LOG_DEVREG) << "Unexpected device VID and PID";
        setError(BackendError::InvalidDevice);

    } else {
        setQueryInProgress(true);
        qCDebug(LOG_DEVREG).noquote().nospace()
            << "Detected new device: VID_0x" << QString::number(info.vendorID(), 16) << ":PID_0x" << QString::number(info.productID(), 16);

        auto *fetcher = Zero::AbstractDeviceInfoHelper::create(info, this);
        connect(fetcher, &Zero::AbstractDeviceInfoHelper::finished, this, &DeviceRegistry::processDevice);
        connect(fetcher, &Zero::AbstractDeviceInfoHelper::finished, fetcher, &QObject::deleteLater);
    }
}

void DeviceRegistry::removeDevice(const USBDeviceInfo &info)
{
    const auto it = std::find_if(m_devices.begin(), m_devices.end(), [&](Flipper::FlipperZero *dev) {
        const auto &deviceInfo = dev->deviceState()->deviceInfo().usbInfo;
        return deviceInfo.backendData() == info.backendData();
    });

    if(it != m_devices.end()) {
        const auto idx = std::distance(m_devices.begin(), it);
        auto *device = m_devices.at(idx);

        if(!device->deviceState()->isPersistent()) {
            qCDebug(LOG_DEVREG).noquote().nospace()
                << "Device disconnected: VID_0x" << QString::number(info.vendorID(), 16) << ":PID_0x" << QString::number(info.productID(), 16);

            m_devices.takeAt(idx)->deleteLater();
            emit deviceCountChanged();
            emit currentDeviceChanged();

        } else {
            qCDebug(LOG_DEVREG).noquote().nospace()
                << "Device went offline: VID_0x" << QString::number(info.vendorID(), 16) << ":PID_0x" << QString::number(info.productID(), 16);

            device->deviceState()->setOnline(false);
        }
    }
}

void DeviceRegistry::removeOfflineDevices()
{
    auto it = std::remove_if(m_devices.begin(), m_devices.end(), [](Flipper::FlipperZero *arg) {
        return !arg->deviceState()->isOnline();
    });

    for(const auto end = m_devices.end(); it != end; ++it) {
        qCDebug(LOG_DEVREG).noquote() << "Removed offline device:" << (*it)->deviceState()->name();

        m_devices.erase(it);
        emit deviceCountChanged();

        (*it)->deleteLater();
    }
}

void DeviceRegistry::processDevice()
{
    setQueryInProgress(false);

    auto *fetcher = qobject_cast<Zero::AbstractDeviceInfoHelper*>(sender());
    const auto &info = fetcher->result();

    if(fetcher->isError()) {
        qCDebug(LOG_DEVREG).noquote() << "Device initialization failed:" << fetcher->errorString();
        setError(fetcher->error());
        return;
    }

    const auto it = std::find_if(m_devices.begin(), m_devices.end(), [&info](Flipper::FlipperZero *arg) {
        return info.name == arg->deviceState()->name();
    });

    if(it != m_devices.end()) {
        // Preserving the old instance
        qCDebug(LOG_DEVREG) << "Device went back online";
        (*it)->deviceState()->setDeviceInfo(info);

    } else {
        qCDebug(LOG_DEVREG) << "Registering the device";

        auto *device = new FlipperZero(info, this);
        m_devices.append(device);

        emit deviceCountChanged();

        if(m_devices.size() == 1) {
            emit currentDeviceChanged();
        }
    }
}

void DeviceRegistry::setError(BackendError::ErrorType newError)
{
    if(m_error == newError) {
        return;
    }

    m_error = newError;
    emit errorOccured();
}

void DeviceRegistry::setQueryInProgress(bool set)
{
    if(m_isQueryInProgress == set) {
        return;
    }

    m_isQueryInProgress = set;
    emit isQueryInProgressChanged();
}

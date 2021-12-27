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

Q_LOGGING_CATEGORY(CAT_DEVREG, "DEVREG");

using namespace Flipper;

DeviceRegistry::DeviceRegistry(QObject *parent):
    QObject(parent)
{
    connect(USBDeviceDetector::instance(), &USBDeviceDetector::devicePluggedIn, this, &DeviceRegistry::insertDevice);
    connect(USBDeviceDetector::instance(), &USBDeviceDetector::deviceUnplugged, this, &DeviceRegistry::removeDevice);

    USBDeviceDetector::instance()->setWantedDevices({
        USBDeviceInfo(FLIPPER_ZERO_VID, FLIPPER_ZERO_PID_DFU),
        USBDeviceInfo(FLIPPER_ZERO_VID, FLIPPER_ZERO_PID_VCP)
            .withManufacturer("Flipper Devices Inc.")
            .withProductDescription("Flipper Control Virtual ComPort")
    });
}

FlipperZero *DeviceRegistry::currentDevice() const
{
    return m_devices.isEmpty() ? nullptr : m_devices.first();
}

void DeviceRegistry::insertDevice(const USBDeviceInfo &info)
{
    if(!info.isValid()) {
        qCDebug(CAT_DEVREG) << "A new invalid device has been detected, skipping...";

    } else if(info.vendorID() != FLIPPER_ZERO_VID) {
        qCDebug(CAT_DEVREG) << "Unexpected device VID and PID";

    } else {
        qCDebug(CAT_DEVREG).noquote().nospace()
            << "Detected new device: VID_0x" << QString::number(info.vendorID(), 16) << ":PID_0x" << QString::number(info.productID(), 16);

        auto *fetcher = Zero::AbstractDeviceInfoHelper::create(info, this);
        connect(fetcher, &Zero::AbstractDeviceInfoHelper::finished, this, &DeviceRegistry::processDevice);
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
            m_devices.takeAt(idx)->deleteLater();
            emit deviceCountChanged();
            emit currentDeviceChanged();

        } else {
            device->deviceState()->setOnline(false);
        }
    }
}

void DeviceRegistry::cleanupOffline()
{
    auto it = std::remove_if(m_devices.begin(), m_devices.end(), [](Flipper::FlipperZero *arg) {
        return !arg->deviceState()->isOnline();
    });

    for(const auto end = m_devices.end(); it != end; ++it) {
        qCDebug(CAT_DEVREG).noquote() << "Removed zombie device:" << (*it)->deviceState()->name();

        m_devices.erase(it);
        emit deviceCountChanged();

        (*it)->deleteLater();
    }
}

void DeviceRegistry::processDevice()
{
    auto *fetcher = qobject_cast<Zero::AbstractDeviceInfoHelper*>(sender());

    fetcher->deleteLater();

    if(fetcher->isError()) {
        qCDebug(CAT_DEVREG).noquote() << "Device initialization failed:" << fetcher->errorString();
        return;
    }

    const auto &info = fetcher->result();

    const auto it = std::find_if(m_devices.begin(), m_devices.end(), [&info](Flipper::FlipperZero *arg) {
        return info.name == arg->deviceState()->name();
    });

    if(it != m_devices.end()) {
        // Preserving the old instance
        (*it)->deviceState()->setDeviceInfo(info);

    } else {
        auto *device = new FlipperZero(info, this);
        m_devices.append(device);

        emit deviceCountChanged();

        if(m_devices.size() == 1) {
            emit currentDeviceChanged();
        }
    }

}

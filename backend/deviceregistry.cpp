#include "deviceregistry.h"

#include <QMetaObject>

#include "flipperzero/helper/deviceinfohelper.h"
#include "flipperzero/flipperzero.h"
#include "flipperzero/devicestate.h"

#include "usbdevice.h"
#include "macros.h"

#define FLIPPER_ZERO_VID 0x0483
#define FLIPPER_ZERO_PID_VCP 0x5740
#define FLIPPER_ZERO_PID_DFU 0xdf11

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
    check_return_void(info.isValid(), "A new invalid device has been detected, skipping...");

    if(info.vendorID() == FLIPPER_ZERO_VID) {
        auto *fetcher = Zero::AbstractDeviceInfoHelper::create(info, this);
        connect(fetcher, &Zero::AbstractDeviceInfoHelper::finished, this, &DeviceRegistry::processDevice);

    } else {
        error_msg("Unexpected device VID and PID.");
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
            emit devicesChanged();

        } else {
            device->deviceState()->setOnline(false);
        }
    }
}

void DeviceRegistry::processDevice()
{
    auto *fetcher = qobject_cast<Zero::AbstractDeviceInfoHelper*>(sender());

    fetcher->deleteLater();

    if(fetcher->isError()) {
        error_msg(QStringLiteral("An error has occured: %1").arg(fetcher->errorString()));
        return;
    }

    const auto &info = fetcher->result();

    const auto it = std::find_if(m_devices.begin(), m_devices.end(), [&info](Flipper::FlipperZero *arg) {
        return info.name == arg->deviceState()->name();
    });

    if(it != m_devices.end()) {
        // Preserving the old instance
        (*it)->deviceState()->reset(info);

    } else {
        auto *device = new FlipperZero(info, this);
        m_devices.append(device);

        emit deviceConnected(device);
        emit devicesChanged();
    }

}

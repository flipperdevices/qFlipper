#include "deviceregistry.h"

#include <QMetaObject>

#include "flipperzero/deviceinfofetcher.h"
#include "flipperzero/flipperzero.h"
#include "usbdevice.h"
#include "macros.h"

#define FLIPPER_ZERO_VID 0x0483
#define FLIPPER_ZERO_PID_VCP 0x5740
#define FLIPPER_ZERO_PID_DFU 0xdf11

using namespace Flipper;

DeviceRegistry::DeviceRegistry(QObject *parent):
    QAbstractListModel(parent)
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

int DeviceRegistry::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.size();
}

QVariant DeviceRegistry::data(const QModelIndex &index, int role) const
{
    return (role == DeviceRole) ? QVariant::fromValue(m_data.at(index.row())) : QVariant();
}

QHash<int, QByteArray> DeviceRegistry::roleNames() const
{
    return { { DeviceRole, "device" } };
}

void DeviceRegistry::insertDevice(const USBDeviceInfo &info)
{
    check_return_void(info.isValid(), "A new invalid device has been detected, skipping...");

    if(info.vendorID() == FLIPPER_ZERO_VID) {
        auto *fetcher = Zero::AbstractDeviceInfoFetcher::create(info, this);
        connect(fetcher, &Zero::AbstractDeviceInfoFetcher::finished, this, &DeviceRegistry::processDevice);
        fetcher->fetch();

    } else {
        error_msg("Unexpected device VID and PID.");
    }
}

void DeviceRegistry::removeDevice(const USBDeviceInfo &info)
{
    const auto it = std::find_if(m_data.begin(), m_data.end(), [&](Flipper::FlipperZero *dev) {
        return dev->deviceInfo().usbInfo.backendData() == info.backendData();
    });

    if(it != m_data.end()) {
        const auto idx = std::distance(m_data.begin(), it);
        auto *device = m_data.at(idx);

        if(!device->isPersistent()) {
            beginRemoveRows(QModelIndex(), idx, idx);
            m_data.takeAt(idx)->deleteLater();
            endRemoveRows();

        } else {
            device->setOnline(false);
        }
    }
}

void DeviceRegistry::processDevice()
{
    auto *fetcher = qobject_cast<Zero::AbstractDeviceInfoFetcher*>(sender());

    if(fetcher->isError()) {
        error_msg(QStringLiteral("An error has occured: %1").arg(fetcher->errorString()));
        return;
    }

    const auto &info = fetcher->result();

    const auto it = std::find_if(m_data.begin(), m_data.end(), [&info](Flipper::FlipperZero *arg) {
        return info.name == arg->name();
    });

    if(it != m_data.end()) {
        // Preserving the old instance
        (*it)->reset(info);

    } else {
        auto *device = new FlipperZero(info, this);

        beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
        m_data.append(device);
        endInsertRows();

        emit deviceConnected(device);
    }

    fetcher->deleteLater();
}

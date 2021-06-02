#include "deviceregistry.h"

#include "flipperzero.h"
#include "usbdevice.h"

using namespace Flipper;

DeviceRegistry::DeviceRegistry(QObject *parent):
    QAbstractListModel(parent)
{
    connect(USBDevice::detector(), &USBDEviceDetector::devicePluggedIn, this, &DeviceRegistry::insertDevice);
    connect(USBDevice::detector(), &USBDEviceDetector::deviceUnplugged, this, &DeviceRegistry::removeDevice);

    USBDevice::detector()->registerHotplugEvent({
        // Flipper Zero in DFU mode
        {
            0x0483,
            0xdf11,
            "",
            "",
            "",
            nullptr
        },

        // Flipper Zero in VCP mode
        {
            0x0483,
            0x5740,
            "Flipper Devices Inc.",
            "Flipper Control Virtual ComPort",
            "",
            nullptr
        }
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
    return {
        { DeviceRole, "device" }
    };
}

void DeviceRegistry::insertDevice(const USBDeviceParams &parameters)
{
    auto *device = new Flipper::Zero(parameters, this);

    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data.append(device);
    endInsertRows();

    emit deviceConnected(device);
}

void DeviceRegistry::removeDevice(const USBDeviceParams &parameters)
{
    const auto it = std::find_if(m_data.begin(), m_data.end(), [&](Flipper::Zero *dev) {
        return dev->uniqueID() == parameters.uniqueID;
    });

    if(it != m_data.end()) {
        const auto idx = std::distance(m_data.begin(), it);

        beginRemoveRows(QModelIndex(), idx, idx);
        m_data.takeAt(idx)->deleteLater();
        endRemoveRows();
    }
}

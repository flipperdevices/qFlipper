#include "registry.h"

#include "flipperzero.h"
#include "usbdevice.h"

using namespace Flipper;

Registry::Registry(QObject *parent):
    QAbstractListModel(parent)
{
    // Queued connection in order to isolate from backend callback context
    connect(&USBDevice::backend(), &USBBackend::devicePluggedIn, this, &Registry::insertDevice, Qt::QueuedConnection);
    connect(&USBDevice::backend(), &USBBackend::deviceUnplugged, this, &Registry::removeDevice, Qt::QueuedConnection);

    const USBBackend::DeviceList params = {
        {
            0x0483,
            0xdf11,
            "",
            "",
            "",
            nullptr
        },

        {
            0x0483,
            0x5740,
            "Flipper Devices Inc.",
            "Flipper Control Virtual ComPort",
            "",
            nullptr
        }
    };

    USBDevice::backend().registerHotplugEvent(params);
}

int Registry::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.size();
}

QVariant Registry::data(const QModelIndex &index, int role) const
{
    return (role == Qt::DisplayRole) ? QVariant::fromValue(m_data.at(index.row())) : QVariant();
}

void Registry::insertDevice(const USBDeviceParams parameters)
{
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data.append(new Flipper::Zero(parameters, this));
    endInsertRows();
}

void Registry::removeDevice(const USBDeviceParams parameters)
{
    const auto it = std::find_if(m_data.begin(), m_data.end(), [&](Flipper::Zero *dev) {
        return dev->uniqueID() == parameters.uniqueID;
    });

    if(it != m_data.end()) {
        const auto idx = std::distance(m_data.begin(), it);

        beginRemoveRows(QModelIndex(), idx, idx);
        m_data.erase(it);
        endRemoveRows();

        (*it)->deleteLater();
    }
}

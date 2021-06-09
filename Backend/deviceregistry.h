#ifndef FLIPPERREGISTRY_H
#define FLIPPERREGISTRY_H

#include <QAbstractListModel>
#include <QVector>

#include "usbdeviceinfo.h"

namespace Flipper {

class Zero;

class DeviceRegistry : public QAbstractListModel
{
    Q_OBJECT

public:
    DeviceRegistry(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    enum Role {
        DeviceRole = Qt::UserRole + 1
    };

signals:
    void deviceConnected(Flipper::Zero*);

public slots:
    void insertDevice(const USBDeviceInfo &parameters);
    void removeDevice(const USBDeviceInfo &parameters);

private:
    QVector<Flipper::Zero*> m_data;
};

}

#endif // FLIPPERREGISTRY_H

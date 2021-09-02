#ifndef FLIPPERREGISTRY_H
#define FLIPPERREGISTRY_H

#include <QAbstractListModel>
#include <QVector>
#include <QQueue>

#include "usbdeviceinfo.h"

namespace Flipper {

class FlipperZero;

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
    void deviceConnected(Flipper::FlipperZero*);

public slots:
    void insertDevice(const USBDeviceInfo &info);
    void removeDevice(const USBDeviceInfo &info);

private slots:
    void processDevice();

private:
    QVector<Flipper::FlipperZero*> m_data;
};

}

#endif // FLIPPERREGISTRY_H

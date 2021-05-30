#ifndef FLIPPERREGISTRY_H
#define FLIPPERREGISTRY_H

#include <QAbstractListModel>
#include <QVector>

#include "usbdeviceparams.h"

namespace Flipper {

class Zero;

class Registry : public QAbstractListModel
{
    Q_OBJECT

public:
    Registry(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public slots:
    void insertDevice(const USBDeviceParams parameters);
    void removeDevice(const USBDeviceParams parameters);

private:
    QVector<Flipper::Zero*> m_data;
};

}

#endif // FLIPPERREGISTRY_H

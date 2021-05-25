#ifndef FLIPPERLISTMODEL_H
#define FLIPPERLISTMODEL_H

#include <QVector>
#include <QAbstractListModel>

#include "flipperinfo.h"

class FlipperListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool empty READ isEmpty NOTIFY contentChanged)

public:
    enum Roles {
        InfoRole = Qt::UserRole + 1,
        ModelRole,
        NameRole,
        VersionRole,
        ProgressRole,
        StatusMessageRole
    };

    FlipperListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    bool isEmpty() const;

signals:
    void contentChanged(bool) const;
    void deviceFound(const FlipperInfo&) const;

public slots:
    void insertDevice(const FlipperInfo &info);
    void removeDevice(const FlipperInfo &info);
    void updateDevice(const FlipperInfo &info);
    void updateDeviceStatus(const FlipperInfo &info);

    void requestDevice(const QString &serialNumber) const;

private:
    QVector<FlipperInfo> m_infos;
};

#endif // FLIPPERLISTMODEL_H

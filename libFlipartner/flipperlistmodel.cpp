#include "flipperlistmodel.h"

FlipperListModel::FlipperListModel(QObject *parent):
    QAbstractListModel(parent)
{}

bool FlipperListModel::isEmpty() const
{
    return m_infos.isEmpty();
}

int FlipperListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_infos.size();
}

QVariant FlipperListModel::data(const QModelIndex &index, int role) const
{
    const auto &item = m_infos.at(index.row());

    switch (role) {
        case InfoRole: return QVariant::fromValue<FlipperInfo>(item);
        case ModelRole: return item.model;
        case NameRole: return item.name;
        case VersionRole: return item.version;
        case ProgressRole: return item.status.progress;
        case StatusMessageRole: return item.status.message;
        case DFUModeRole: return item.isDFU();
        default: return QVariant();
    }
}

QHash<int, QByteArray> FlipperListModel::roleNames() const
{
    static QHash<int, QByteArray> roles {
        {InfoRole, "info"},
        {ModelRole, "model"},
        {NameRole, "name"},
        {VersionRole, "version"},
        {ProgressRole, "progress"},
        {StatusMessageRole, "message"},
        {DFUModeRole, "dfumode"}
    };

    return roles;
}

void FlipperListModel::insertDevice(const FlipperInfo &info)
{
    beginInsertRows(QModelIndex(), m_infos.size(), m_infos.size());
    m_infos.append(info);
    endInsertRows();

    emit contentChanged(isEmpty());
}

void FlipperListModel::removeDevice(const FlipperInfo &info)
{
    const auto idx = m_infos.indexOf(info);

    if(idx < 0) {
        return;
    }

    beginRemoveRows(QModelIndex(), idx, idx);
    m_infos.remove(idx);
    endRemoveRows();

    emit contentChanged(isEmpty());
}

void FlipperListModel::updateDevice(const FlipperInfo &info)
{
    const auto idx = m_infos.indexOf(info);

    if(idx < 0) {
        return;
    }

    auto newInfo = info;
    newInfo.status = m_infos.at(idx).status;

    m_infos.replace(idx, newInfo);
    emit dataChanged(index(idx), index(idx), {ModelRole, NameRole, VersionRole});
}

void FlipperListModel::updateDeviceStatus(const FlipperInfo &info)
{
    const auto idx = m_infos.indexOf(info);

    if(idx < 0) {
        return;
    }

    m_infos[idx].status = info.status;
    emit dataChanged(index(idx), index(idx), {ProgressRole, StatusMessageRole});
}

void FlipperListModel::requestDevice(const QString &serialNumber) const
{
    for(const auto &info : m_infos) {
        if(info.params.serialNumber == serialNumber) {
            emit deviceFound(info);
            break;
        }
    }
}

#ifndef UPDATESLISTMODEL_H
#define UPDATESLISTMODEL_H

#include <QMap>
#include <QStringList>
#include <QAbstractListModel>

#include "flipperupdates.h"

namespace Flipper {

class UpdateChannelModel: public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(Flipper::Updates::VersionInfo latestVersion READ latestVersion CONSTANT)

public:
    enum Role {
        VersionRole = Qt::UserRole + 1,
        NumberRole,
        TimestampRole,
        ChangelogRole,
    };

    UpdateChannelModel(const Updates::ChannelInfo &channelInfo, QObject *parent = nullptr);
    ~UpdateChannelModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    const QString &name() const;
    const QString &description() const;

    const Flipper::Updates::VersionInfo latestVersion() const;

private:
    Updates::ChannelInfo m_channelInfo;
};

class UpdateRegistry : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList channelNames READ channelNames NOTIFY channelsChanged)

public:

    UpdateRegistry(QObject *parent = nullptr);
    ~UpdateRegistry();

    bool fillFromJson(const QByteArray &text);

    const QStringList channelNames() const;
    Q_INVOKABLE Flipper::UpdateChannelModel *channelModel(const QString &channelName) const;

signals:
    void channelsChanged();

private:
    QMap<QString, UpdateChannelModel*> m_channelModels;
};

}

#endif // UPDATESLISTMODEL_H

#ifndef UPDATESLISTMODEL_H
#define UPDATESLISTMODEL_H

#include <QMap>
#include <QStringList>
#include <QAbstractListModel>

#include "flipperupdates.h"
#include "flipperinfo.h"

class UpdatesListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString channel READ channel WRITE setChannel NOTIFY channelChanged)
    Q_PROPERTY(QString target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QStringList channels READ channels NOTIFY channelsChanged)
    Q_PROPERTY(FlipperInfo targetDevice WRITE setTargetDevice)

public:
    enum Role {
        VersionRole = Qt::UserRole + 1,
        TimestampRole,
        ChangelogRole,
        FileRole
    };

    UpdatesListModel(QObject *parent = nullptr);
    ~UpdatesListModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    const QStringList channels() const;

    const QString &channel() const;
    void setChannel(const QString &name);

    const QString &target() const;
    void setTarget(const QString &name);

    void setTargetDevice(const FlipperInfo &info);

    bool fillFromJson(const QByteArray &text);

signals:
    void channelsChanged(const QStringList);
    void channelChanged(const QString&);
    void targetChanged(const QString&);

private:
    QMap<QString, FlipperUpdates::ChannelInfo> m_channels;
    QString m_currentChannel;
    QString m_currentTarget;
};

#endif // UPDATESLISTMODEL_H

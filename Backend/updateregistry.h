#ifndef UPDATESLISTMODEL_H
#define UPDATESLISTMODEL_H

#include <QMap>
#include <QStringList>
#include <QAbstractListModel>

#include "flipperupdates.h"

namespace Flipper {

// TODO: Split into 2 separate models: files model and version model

class UpdateRegistry : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString channel READ channel WRITE setChannel NOTIFY channelChanged)
    Q_PROPERTY(QString channelDescription READ channelDescription NOTIFY channelDescriptionChanged)
    Q_PROPERTY(QString target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QStringList channels READ channels NOTIFY channelsChanged)

public:
    enum Role {
        VersionRole = Qt::UserRole + 1,
        TimestampRole,
        ChangelogRole,
        FileRole
    };

    UpdateRegistry(QObject *parent = nullptr);
    ~UpdateRegistry();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    const QStringList channels() const;
    const QString channelDescription() const;

    const QString &channel() const;
    const QString &target() const;

    Q_INVOKABLE const QString latestVersion(const QString &target) const;
    Q_INVOKABLE Updates::FileInfo latestFirmware(const QString &target) const;

    void setChannel(const QString &name);
    void setTarget(const QString &name);

    bool fillFromJson(const QByteArray &text);

signals:
    void channelsChanged(const QStringList&);
    void channelChanged(const QString&);
    void channelDescriptionChanged(const QString &);
    void targetChanged(const QString&);

private:
    QMap<QString, Updates::ChannelInfo> m_channels;
    QString m_currentChannel;
    QString m_currentTarget;
};

}

#endif // UPDATESLISTMODEL_H

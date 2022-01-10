#pragma once

#include <QMap>
#include <QAbstractListModel>

#include "flipperupdates.h"

class QTimer;

namespace Flipper {

class UpdateRegistry : public QAbstractListModel
{
    Q_OBJECT
    using ChannelMap = QMap<QString, Updates::ChannelInfo>;

    enum DataRole {
        NameRole,
        TitleRole,
        DescriptionRole
    };

public:
    UpdateRegistry(const QString &directoryUrl, QObject *parent = nullptr);

    void setDirectoryUrl(const QString &directoryUrl);
    bool fillFromJson(const QByteArray &text);

    bool isReady() const;
    const QStringList channelNames() const;

    const Flipper::Updates::VersionInfo latestVersion() const;
    const Flipper::Updates::ChannelInfo channel(const QString &channelName) const;

    // Model API functions
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void check();

signals:
    void channelsChanged();
    void latestVersionChanged();

private:
    virtual const QString updateChannel() const = 0;

    QString m_directoryUrl;
    QTimer *m_checkTimer;
    ChannelMap m_channels;
};
}

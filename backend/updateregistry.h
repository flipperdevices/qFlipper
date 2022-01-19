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
    enum class State {
        Unknown,
        Checking,
        Ready,
        ErrorOccured
    };

    UpdateRegistry(const QString &directoryUrl, QObject *parent = nullptr);

    void setDirectoryUrl(const QString &directoryUrl);
    void fillFromJson(const QByteArray &text);

    State state() const;
    const QStringList channelNames() const;

    const Flipper::Updates::VersionInfo latestVersion() const;
    const Flipper::Updates::ChannelInfo channel(const QString &channelName) const;

    // Model API functions
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void stateChanged();
    void latestVersionChanged();

public slots:
    void check();

private:
    virtual const QString updateChannel() const = 0;
    void setState(State newState);

    QString m_directoryUrl;
    QTimer *m_checkTimer;
    ChannelMap m_channels;
    State m_state;
};
}

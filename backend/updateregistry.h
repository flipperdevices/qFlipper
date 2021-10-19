#ifndef UPDATESLISTMODEL_H
#define UPDATESLISTMODEL_H

#include <QMap>

#include "flipperupdates.h"

namespace Flipper {

class UpdateRegistry : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList channelNames READ channelNames NOTIFY channelsChanged)
    Q_PROPERTY(Flipper::Updates::VersionInfo latestVersion READ latestVersion NOTIFY channelsChanged)
    Q_PROPERTY(bool isReady READ isReady NOTIFY channelsChanged)

public:
    UpdateRegistry(const QString &directoryUrl, QObject *parent = nullptr);
    bool fillFromJson(const QByteArray &text);

    const QStringList channelNames() const;
    bool isReady() const;

    const Flipper::Updates::VersionInfo latestVersion() const;

public slots:
    Flipper::Updates::ChannelInfo channel(const QString &channelName) const;

signals:
    void channelsChanged();

private:
    QMap<QString, Updates::ChannelInfo> m_channels;
};

}

#endif // UPDATESLISTMODEL_H

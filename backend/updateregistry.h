#pragma once

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
    virtual const QString updateChannel() const = 0;
    QMap<QString, Updates::ChannelInfo> m_channels;
};

class FirmwareUpdates : public UpdateRegistry {
    Q_OBJECT

public:
    FirmwareUpdates(const QString &directoryUrl, QObject *parent = nullptr):
        UpdateRegistry(directoryUrl, parent) {}

private:
    const QString updateChannel() const override;
};

class ApplicationUpdates : public UpdateRegistry {
    Q_OBJECT

public:
    ApplicationUpdates(const QString &directoryUrl, QObject *parent = nullptr):
        UpdateRegistry(directoryUrl, parent) {}

private:
    const QString updateChannel() const override;
};

}

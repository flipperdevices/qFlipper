#pragma once

#include <QMap>

#include "flipperupdates.h"

class QTimer;

namespace Flipper {

class UpdateRegistry : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList channelNames READ channelNames NOTIFY channelsChanged)
    Q_PROPERTY(Flipper::Updates::VersionInfo latestVersion READ latestVersion NOTIFY latestVersionChanged)
    Q_PROPERTY(bool isReady READ isReady NOTIFY channelsChanged)

    using ChannelMap = QMap<QString, Updates::ChannelInfo>;

public:
    UpdateRegistry(const QString &directoryUrl, QObject *parent = nullptr);
    bool fillFromJson(const QByteArray &text);

    const QStringList channelNames() const;
    bool isReady() const;

    const Flipper::Updates::VersionInfo latestVersion() const;
    Q_INVOKABLE Flipper::Updates::ChannelInfo channel(const QString &channelName) const;

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

class FirmwareUpdates : public UpdateRegistry {
    Q_OBJECT

public:
    FirmwareUpdates(const QString &directoryUrl, QObject *parent = nullptr);

private:
    const QString updateChannel() const override;
};

class ApplicationUpdates : public UpdateRegistry {
    Q_OBJECT

public:
    ApplicationUpdates(const QString &directoryUrl, QObject *parent = nullptr);

private:
    const QString updateChannel() const override;
};

}

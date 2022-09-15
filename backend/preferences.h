#pragma once

#include <QUrl>
#include <QObject>
#include <QSettings>

class Preferences : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString updateChannel READ firmwareUpdateChannel WRITE setFirmwareUpdateChannel NOTIFY firmwareUpdateChannelChanged)
    Q_PROPERTY(QString appUpdateChannel READ applicationUpdateChannel WRITE setApplicationUpdateChannel NOTIFY applicationUpdateChannelChanged)
    Q_PROPERTY(bool checkAppUpdates READ checkApplicationUpdates WRITE setCheckApplicationUpdates NOTIFY checkApplicationUpdatesChanged)
    Q_PROPERTY(bool showHiddenFiles READ showHiddenFiles WRITE setShowHiddenFiles NOTIFY showHiddenFilesChanged)

    Preferences(QObject *parent = nullptr);

public:
    static Preferences *instance();

    const QString firmwareUpdateChannel() const;
    void setFirmwareUpdateChannel(const QString &newUpdateChannel);

    const QString applicationUpdateChannel() const;
    void setApplicationUpdateChannel(const QString &newUpdateChannel);

    bool checkApplicationUpdates() const;
    void setCheckApplicationUpdates(bool set);

    bool showHiddenFiles() const;
    void setShowHiddenFiles(bool set);

    QUrl lastFolderUrl() const;
    void setLastFolderUrl(const QUrl &url);

signals:
    void firmwareUpdateChannelChanged();
    void applicationUpdateChannelChanged();
    void checkApplicationUpdatesChanged();
    void showHiddenFilesChanged();
    void lastFolderUrlChanged();

private:
    QSettings m_settings;
};

#define globalPrefs (Preferences::instance())


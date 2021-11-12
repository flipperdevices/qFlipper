#pragma once

#include <QObject>
#include <QSettings>

class Preferences : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString updateChannel READ firmwareUpdateChannel WRITE setFirmwareUpdateChannel NOTIFY firmwareUpdateChannelChanged)
    Q_PROPERTY(QString appUpdateChannel READ applicationUpdateChannel WRITE setApplicationUpdateChannel NOTIFY applicationUpdateChannelChanged)
    Q_PROPERTY(bool checkAppUpdates READ checkApplicationUpdates WRITE setCheckApplicationUpdates NOTIFY checkApplicationUpdatesChanged)

    Preferences(QObject *parent = nullptr);

public:
    static Preferences *instance();

    const QString firmwareUpdateChannel() const;
    void setFirmwareUpdateChannel(const QString &newUpdateChannel);

    const QString applicationUpdateChannel() const;
    void setApplicationUpdateChannel(const QString &newUpdateChannel);

    bool checkApplicationUpdates() const;
    void setCheckApplicationUpdates(bool set);

signals:
    void firmwareUpdateChannelChanged();
    void applicationUpdateChannelChanged();
    void checkApplicationUpdatesChanged();

private:
    QSettings m_settings;
};

#define globalPrefs() (Preferences::instance())


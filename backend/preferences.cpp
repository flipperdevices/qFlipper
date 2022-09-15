#include "preferences.h"

#include <QStandardPaths>

#define FIRMWARE_UPDATE_CHANNEL_KEY (QStringLiteral("FirmwareUpdateChannel"))
#define APPLICATION_UPDATE_CHANNEL_KEY (QStringLiteral("ApplicationUpdateChannel"))
#define CHECK_APPLICATION_UPDATES_KEY (QStringLiteral("CheckApplicatonUpdates"))
#define SHOW_HIDDEN_FILES_KEY (QStringLiteral("ShowHiddenFiles"))
#define LAST_FOLDER_URL_KEY (QStringLiteral("LastFolderUrl"))

#define SET_DEFAULT_VALUE(key, value)\
    if(!m_settings.contains(key)) {\
        m_settings.setValue(key, value);\
    }

Preferences::Preferences(QObject *parent):
    QObject(parent)
{
    SET_DEFAULT_VALUE(FIRMWARE_UPDATE_CHANNEL_KEY, QStringLiteral("release"));
    SET_DEFAULT_VALUE(APPLICATION_UPDATE_CHANNEL_KEY, QStringLiteral("release"));
    SET_DEFAULT_VALUE(CHECK_APPLICATION_UPDATES_KEY, true);
    SET_DEFAULT_VALUE(SHOW_HIDDEN_FILES_KEY, false);
    SET_DEFAULT_VALUE(LAST_FOLDER_URL_KEY, QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
}

Preferences *Preferences::instance()
{
    static auto *prefs = new Preferences();
    return prefs;
}

const QString Preferences::firmwareUpdateChannel() const
{
    return m_settings.value(FIRMWARE_UPDATE_CHANNEL_KEY).toString();
}

void Preferences::setFirmwareUpdateChannel(const QString &newUpdateChannel)
{
    if(newUpdateChannel == firmwareUpdateChannel()) {
        return;
    }

    m_settings.setValue(FIRMWARE_UPDATE_CHANNEL_KEY, newUpdateChannel);
    emit firmwareUpdateChannelChanged();
}

const QString Preferences::applicationUpdateChannel() const
{
    return m_settings.value(APPLICATION_UPDATE_CHANNEL_KEY).toString();
}

void Preferences::setApplicationUpdateChannel(const QString &newUpdateChannel)
{
    if(newUpdateChannel == applicationUpdateChannel()) {
        return;
    }

    m_settings.setValue(APPLICATION_UPDATE_CHANNEL_KEY, newUpdateChannel);
    emit applicationUpdateChannelChanged();
}

bool Preferences::checkApplicationUpdates() const
{
#ifndef DISABLE_APPLICATION_UPDATES
    return m_settings.value(CHECK_APPLICATION_UPDATES_KEY).toBool();
#else
    return false;
#endif
}

void Preferences::setCheckApplicationUpdates(bool set)
{
    if(set == checkApplicationUpdates()) {
        return;
    }

    m_settings.setValue(CHECK_APPLICATION_UPDATES_KEY, set);
    emit checkApplicationUpdatesChanged();
}

bool Preferences::showHiddenFiles() const
{
    return m_settings.value(SHOW_HIDDEN_FILES_KEY).toBool();
}

void Preferences::setShowHiddenFiles(bool set)
{
    if(set == showHiddenFiles()) {
        return;
    }

    m_settings.setValue(SHOW_HIDDEN_FILES_KEY, set);
    emit showHiddenFilesChanged();
}

QUrl Preferences::lastFolderUrl() const
{
    return QUrl::fromLocalFile(m_settings.value(LAST_FOLDER_URL_KEY).toString());
}

void Preferences::setLastFolderUrl(const QUrl &url)
{
    if(url == lastFolderUrl()) {
        return;
    }

    m_settings.setValue(LAST_FOLDER_URL_KEY, url.toLocalFile());
    emit lastFolderUrlChanged();
}

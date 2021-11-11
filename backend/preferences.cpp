#include "preferences.h"

#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>

#define CONFIG_FILE_NAME (qApp->applicationName() + QStringLiteral(".ini"))
#define CONFIG_PATH (QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation))
#define CONFIG_FILE (QDir(CONFIG_PATH).absoluteFilePath(CONFIG_FILE_NAME))

#define FIRMWARE_UPDATE_CHANNEL_KEY (QStringLiteral("FirmwareUpdateChannel"))
#define APPLICATION_UPDATE_CHANNEL_KEY (QStringLiteral("ApplicationUpdateChannel"))
#define CHECK_APPLICATION_UPDATES_KEY (QStringLiteral("CheckApplicatonUpdates"))

Preferences::Preferences(QObject *parent):
    QObject(parent),
    m_settings(CONFIG_FILE, QSettings::IniFormat, this)
{
    if(!m_settings.contains(FIRMWARE_UPDATE_CHANNEL_KEY)) {
        m_settings.setValue(FIRMWARE_UPDATE_CHANNEL_KEY, QStringLiteral("development"));
    }

    if(!m_settings.contains(APPLICATION_UPDATE_CHANNEL_KEY)) {
        m_settings.setValue(APPLICATION_UPDATE_CHANNEL_KEY, QStringLiteral("development"));
    }

    if(!m_settings.contains(CHECK_APPLICATION_UPDATES_KEY)) {
        m_settings.setValue(CHECK_APPLICATION_UPDATES_KEY, true);
    }
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
    return m_settings.value(CHECK_APPLICATION_UPDATES_KEY).toBool();
}

void Preferences::setCheckApplicationUpdates(bool set)
{
    if(set == checkApplicationUpdates()) {
        return;
    }

    m_settings.setValue(CHECK_APPLICATION_UPDATES_KEY, set);
    emit checkApplicationUpdatesChanged();
}

#include "updateregistry.h"

#include <stdexcept>

#include <QLoggingCategory>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QBuffer>
#include <QTimer>
#include <QDebug>

#include "debug.h"
#include "preferences.h"
#include "remotefilefetcher.h"

Q_LOGGING_CATEGORY(CATEGORY_UPDATES, "UPDATES")

using namespace Flipper;

UpdateRegistry::UpdateRegistry(const QString &directoryUrl, QObject *parent):
    QObject(parent),
    m_directoryUrl(directoryUrl),
    m_checkTimer(new QTimer(this))
{
    connect(this, &UpdateRegistry::channelsChanged, this, &UpdateRegistry::latestVersionChanged);
    connect(m_checkTimer, &QTimer::timeout, this, &UpdateRegistry::check);

    check();

    m_checkTimer->start(std::chrono::minutes(10));
}

bool UpdateRegistry::fillFromJson(const QByteArray &text)
{
    m_channels.clear();

    const auto doc = QJsonDocument::fromJson(text);

    check_return_bool(!doc.isNull(), "Failed to parse the document");
    check_return_bool(doc.isObject(),"Json document is not an object");

    const auto &obj = doc.object();

    check_return_bool(obj.contains("channels"), "No channels data in json file");
    check_return_bool(obj["channels"].isArray(), "Expected to get an array of channels");

    const auto &arr = obj["channels"].toArray();

    try {

        for(const auto &val : arr) {
            const Updates::ChannelInfo info(val);
            m_channels.insert(info.name(), info);
        }

        return true;

    } catch(std::runtime_error &e) {
        error_msg(e.what());
        return false;
    }
}

const QStringList UpdateRegistry::channelNames() const
{
    return m_channels.keys();
}

bool UpdateRegistry::isReady() const
{
    return !m_channels.isEmpty();
}

const Updates::VersionInfo UpdateRegistry::latestVersion() const
{
    return channel(updateChannel()).latestVersion();
}

Updates::ChannelInfo UpdateRegistry::channel(const QString &channelName) const
{
    return m_channels.value(channelName);
}

void UpdateRegistry::check()
{
    auto *fetcher = new RemoteFileFetcher(this);
    auto *buf = new QBuffer(this);

    fetcher->connect(fetcher, &RemoteFileFetcher::finished, this, [=]() {
        if(fetcher->isError()) {

        } else if(buf->open(QIODevice::ReadOnly)) {
            qCDebug(CATEGORY_UPDATES).noquote() << "Fetched update directory from" << m_directoryUrl;

            if(fillFromJson(buf->readAll())) {
                emit channelsChanged();
            }

        } else {
            qCCritical(CATEGORY_UPDATES).noquote() << "Failed to open a buffer for reading:" << buf->errorString();
        }

        fetcher->deleteLater();
        buf->deleteLater();
    });

    if(!fetcher->fetch(m_directoryUrl, buf)) {
        buf->deleteLater();
    }
}

FirmwareUpdates::FirmwareUpdates(const QString &directoryUrl, QObject *parent):
    UpdateRegistry(directoryUrl, parent)
{
    connect(globalPrefs, &Preferences::firmwareUpdateChannelChanged, this, &UpdateRegistry::latestVersionChanged);
}

const QString FirmwareUpdates::updateChannel() const
{
    return globalPrefs->firmwareUpdateChannel();
}

ApplicationUpdates::ApplicationUpdates(const QString &directoryUrl, QObject *parent):
    UpdateRegistry(directoryUrl, parent)
{
    connect(globalPrefs, &Preferences::applicationUpdateChannelChanged, this, &UpdateRegistry::latestVersionChanged);
}

const QString ApplicationUpdates::updateChannel() const
{
    return globalPrefs->applicationUpdateChannel();
}

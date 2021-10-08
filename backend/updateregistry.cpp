#include "updateregistry.h"

#include <stdexcept>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QBuffer>

#include "macros.h"
#include "preferences.h"
#include "remotefilefetcher.h"

using namespace Flipper;

UpdateRegistry::UpdateRegistry(const QString &directoryUrl, QObject *parent):
    QObject(parent)
{
    auto *fetcher = new RemoteFileFetcher(this);
    auto *buf = new QBuffer(this);

    fetcher->connect(fetcher, &RemoteFileFetcher::finished, this, [=]() {
        if(buf->open(QIODevice::ReadOnly)) {
            info_msg(QStringLiteral("Fetched update directory from %1.").arg(directoryUrl));
            fillFromJson(buf->readAll());

        } else {
            info_msg(QStringLiteral("Failed to open a buffer for reading: %1.").arg(buf->errorString()));
        }

        fetcher->deleteLater();
        buf->deleteLater();
    });

    if(!fetcher->fetch(directoryUrl, buf)) {
        buf->deleteLater();
    }
}

bool UpdateRegistry::fillFromJson(const QByteArray &text)
{
    // TODO: Clear map first
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

    } catch(std::runtime_error &e) {
        error_msg(e.what());
        return false;
    }

    emit channelsChanged();
    return true;
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
    return channel(Preferences::instance()->firmwareUpdateChannel()).latestVersion();
}

Updates::ChannelInfo UpdateRegistry::channel(const QString &channelName) const
{
    return m_channels.value(channelName);
}

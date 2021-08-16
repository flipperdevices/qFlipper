#include "updateregistry.h"

#include <stdexcept>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

#include "macros.h"
#include "remotefilefetcher.h"

using namespace Flipper;

UpdateRegistry::UpdateRegistry(QObject *parent):
    QObject(parent)
{
    auto *fetcher = new RemoteFileFetcher(this);

    fetcher->connect(fetcher, &RemoteFileFetcher::finished, this, [=](const QByteArray &data) {
        if(!data.isEmpty()) {
            info_msg("Fetched update list from server");
            fillFromJson(data);
        }

        fetcher->deleteLater();
    });

    fetcher->fetch("https://update.flipperzero.one/firmware/directory.json");
}

UpdateRegistry::~UpdateRegistry()
{}

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

Updates::ChannelInfo UpdateRegistry::channel(const QString &channelName) const
{
    return m_channels.value(channelName);
}

#include "updateregistry.h"

#include <stdexcept>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

#include "macros.h"
#include "remotefilefetcher.h"

using namespace Flipper;

UpdateChannelModel::UpdateChannelModel(const Updates::ChannelInfo &channelInfo, QObject *parent):
    QAbstractListModel(parent),
    m_channelInfo(channelInfo)
{}

UpdateChannelModel::~UpdateChannelModel()
{}

int UpdateChannelModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_channelInfo.versions.size();
}

QVariant UpdateChannelModel::data(const QModelIndex &index, int role) const
{
    const auto row = index.row();
    const auto &version = m_channelInfo.versions.at(row);

    if(role == VersionRole) {
        return QVariant::fromValue(version);
    } else if(role == NumberRole) {
        return version.version;
    } else if(role == TimestampRole) {
        return QDateTime::fromSecsSinceEpoch(version.timestamp).date().toString();
    } else if(role == ChangelogRole) {
        return version.changelog;
    } else {
        return QVariant();
    }
}

QHash<int, QByteArray> UpdateChannelModel::roleNames() const
{
    return QHash<int, QByteArray> {
        {VersionRole, "version"},
        {NumberRole, "number"},
        {TimestampRole, "timestamp"},
        {ChangelogRole, "changelog"},
    };
}

const QString &UpdateChannelModel::name() const
{
    return m_channelInfo.id;
}

const QString &UpdateChannelModel::description() const
{
    return m_channelInfo.description;
}

const Updates::VersionInfo UpdateChannelModel::latestVersion() const
{
    return m_channelInfo.versions.first();
}

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

    fetcher->fetch("https://update.flipperzero.one/directory.json");
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
            m_channelModels.insert(info.id, new UpdateChannelModel(info, this));
        }

    } catch(std::runtime_error &e) {
        error_msg("Unexpected json file");
        return false;
    }

    emit channelsChanged();
    return true;
}

const QStringList UpdateRegistry::channelNames() const
{
    return m_channelModels.keys();
}

UpdateChannelModel *UpdateRegistry::channelModel(const QString &channelName) const
{
    return m_channelModels.value(channelName, nullptr);
}

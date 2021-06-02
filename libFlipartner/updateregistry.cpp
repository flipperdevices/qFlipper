#include "updateregistry.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

#include "macros.h"
#include "remotefilefetcher.h"

using namespace Flipper;

UpdateRegistry::UpdateRegistry(QObject *parent):
    QAbstractListModel(parent)
{
    auto *fetcher = new RemoteFileFetcher(this);

    fetcher->connect(fetcher, &RemoteFileFetcher::finished, this, [=](const QByteArray &data) {
        if(!data.isEmpty()) {
            fillFromJson(data);
        }

        fetcher->deleteLater();
    });

    fetcher->fetch("https://update.flipperzero.one/directory.json");
}

UpdateRegistry::~UpdateRegistry()
{}

int UpdateRegistry::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    if(!m_channels.contains(m_currentChannel)) {
        return 0;
    }

    return m_channels[m_currentChannel].versions.size();
}

QVariant UpdateRegistry::data(const QModelIndex &index, int role) const
{
    const auto row = index.row();
    const auto &channel = m_channels[m_currentChannel];
    const auto &version = channel.versions.at(row);

    if(role == VersionRole) {
        return version.version;
    } else if(role == TimestampRole) {
        return QDateTime::fromSecsSinceEpoch(version.timestamp).date().toString();
    } else if(role == ChangelogRole) {
        return version.changelog;
    } else if(role == FileRole) {
        const auto index = version.indexOf(m_currentTarget, "full_dfu");
        return index < 0 ? QVariant() : QVariant::fromValue(version.files.at(index));
    } else {
        return QVariant();
    }
}

QHash<int, QByteArray> UpdateRegistry::roleNames() const
{
    static QHash<int, QByteArray> roles {
        {VersionRole, "version"},
        {TimestampRole, "timestamp"},
        {ChangelogRole, "changelog"},
        {FileRole, "file"}
    };

    return roles;
}

const QStringList UpdateRegistry::channels() const
{
    return m_channels.keys();
}

const QString &UpdateRegistry::channel() const
{
    return m_currentChannel;
}

const QString UpdateRegistry::channelDescription() const
{
    return m_channels[m_currentChannel].description;
}

void UpdateRegistry::setChannel(const QString &name)
{
    if(name == m_currentChannel) {
        return;
    }

    beginResetModel();
    m_currentChannel = name;
    endResetModel();

    emit channelChanged(m_currentChannel);
    emit channelDescriptionChanged(channelDescription());
}

const QString &UpdateRegistry::target() const
{
    return m_currentTarget;
}

const QString UpdateRegistry::latestVersion(const QString &target) const
{
    for(const auto &version: m_channels["release"].versions) {
        const auto index = version.indexOf(target, "full_dfu");
        if(index >= 0) {
            return version.version;
        }
    }

    return "N/A";
}

Updates::FileInfo UpdateRegistry::latestFirmware(const QString &target) const
{
    for(const auto &version: m_channels["release"].versions) {
        const auto index = version.indexOf(target, "full_dfu");
        if(index >= 0) {
            return version.files.at(index);
        }
    }

    return Updates::FileInfo();
}

void UpdateRegistry::setTarget(const QString &name)
{
    if(name == m_currentTarget) {
        return;
    }

    beginResetModel();
    m_currentTarget = name;
    endResetModel();

    emit targetChanged(m_currentTarget);
}

bool UpdateRegistry::fillFromJson(const QByteArray &text)
{
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
            m_channels.insert(info.id, info);
            emit channelsChanged(channels());
        }

    } catch(std::runtime_error &e) {
        error_msg("Unexpected json file");
        return false;
    }

    if(m_channels.contains("release")) {
        setChannel("release");
    } else if(!m_channels.isEmpty()) {
        setChannel(m_channels.first().id);
    }

    return true;
}

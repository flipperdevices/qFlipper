#include "updateregistry.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

#include "macros.h"

using namespace Flipper;

UpdateRegistry::UpdateRegistry(QObject *parent):
    QAbstractListModel(parent)
{}

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

        qDebug() << m_currentChannel << m_currentTarget;
    if(role == VersionRole) {
        return version.version;
    } else if(role == TimestampRole) {
        return QDateTime::fromSecsSinceEpoch(version.timestamp).date().toString();
    } else if(role == ChangelogRole) {
        return version.changelog;
    } else if(role == FileRole) {
        const auto it = std::find_if(version.files.cbegin(), version.files.cend(),
            [this](const Updates::FileInfo &arg) {
                return (arg.type == "full_dfu") && (m_currentTarget == arg.target);
            });

        return (it != version.files.cend()) ? QVariant::fromValue(*it) : QVariant();
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

void UpdateRegistry::setChannel(const QString &name)
{
    if(name == m_currentChannel) {
        return;
    }

    beginResetModel();
    m_currentChannel = name;
    endResetModel();

    emit channelChanged(m_currentChannel);
}

const QString &UpdateRegistry::target() const
{
    return m_currentTarget;
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

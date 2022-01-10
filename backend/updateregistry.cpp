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
#include "remotefilefetcher.h"

Q_LOGGING_CATEGORY(CATEGORY_UPDATES, "UPDATES")

using namespace Flipper;

UpdateRegistry::UpdateRegistry(const QString &directoryUrl, QObject *parent):
    QAbstractListModel(parent),
    m_directoryUrl(directoryUrl),
    m_checkTimer(new QTimer(this))
{
    connect(this, &UpdateRegistry::channelsChanged, this, &UpdateRegistry::latestVersionChanged);
    connect(m_checkTimer, &QTimer::timeout, this, &UpdateRegistry::check);

    check();
}

void UpdateRegistry::setDirectoryUrl(const QString &directoryUrl)
{
    m_directoryUrl = directoryUrl;
    check();
}

bool UpdateRegistry::fillFromJson(const QByteArray &text)
{
    beginRemoveRows(QModelIndex(), 0, m_channels.size() - 1);
    m_channels.clear();
    endRemoveRows();

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

            beginInsertRows(QModelIndex(), m_channels.size(), m_channels.size());
            m_channels.insert(info.name(), info);
            endInsertRows();
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

const Updates::ChannelInfo UpdateRegistry::channel(const QString &channelName) const
{
    return m_channels.value(channelName);
}

int UpdateRegistry::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_channels.size();
}

QVariant UpdateRegistry::data(const QModelIndex &index, int role) const
{
    const auto it = m_channels.cbegin();
    const auto &channel = *(it+index.row());

    switch(role) {
    case NameRole:
        return channel.name();
    case TitleRole:
        return channel.title();
    case DescriptionRole:
        return channel.description();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> UpdateRegistry::roleNames() const
{
    return {
        { NameRole, QByteArrayLiteral("name") },
        { TitleRole, QByteArrayLiteral("title") },
        { DescriptionRole, QByteArrayLiteral("description") }
    };
}

void UpdateRegistry::check()
{
    if(m_directoryUrl.isEmpty()) {
        return;
    }

    auto *fetcher = new RemoteFileFetcher(this);
    auto *buf = new QBuffer(this);

    fetcher->connect(fetcher, &RemoteFileFetcher::finished, this, [=]() {
        if(fetcher->isError()) {
            qCWarning(CATEGORY_UPDATES).noquote() << "Failed to fetch update list:" << fetcher->errorString();
        } else if(buf->open(QIODevice::ReadOnly)) {
            qCDebug(CATEGORY_UPDATES).noquote() << "Fetched update directory from" << m_directoryUrl;

            if(fillFromJson(buf->readAll())) {
                emit channelsChanged();
            }

        } else {
            qCDebug(CATEGORY_UPDATES).noquote() << "Failed to open a buffer for reading:" << buf->errorString();
        }

        fetcher->deleteLater();
        buf->deleteLater();
    });

    if(!fetcher->fetch(m_directoryUrl, buf)) {
        buf->deleteLater();
    }

    m_checkTimer->start(std::chrono::minutes(10));
}

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

#include "remotefilefetcher.h"

Q_LOGGING_CATEGORY(CATEGORY_UPDATES, "UPD")

using namespace Flipper;

UpdateRegistry::UpdateRegistry(const QString &directoryUrl, QObject *parent):
    QAbstractListModel(parent),
    m_directoryUrl(directoryUrl),
    m_checkTimer(new QTimer(this)),
    m_state(State::Unknown)
{
    connect(this, &UpdateRegistry::stateChanged, this, &UpdateRegistry::latestVersionChanged);
    connect(m_checkTimer, &QTimer::timeout, this, &UpdateRegistry::check);

    check();
}

void UpdateRegistry::setDirectoryUrl(const QString &directoryUrl)
{
    m_directoryUrl = directoryUrl;
    check();
}

void UpdateRegistry::fillFromJson(const QByteArray &text)
{
    beginRemoveRows(QModelIndex(), 0, m_channels.size() - 1);
    m_channels.clear();
    endRemoveRows();

    const auto doc = QJsonDocument::fromJson(text);

    if(doc.isNull()) {
        qCCritical(CATEGORY_UPDATES) << "Failed to parse the document";
        return;
    } else if(!doc.isObject()) {
        qCCritical(CATEGORY_UPDATES) << "Json document is not an object";
        return;
    }

    const auto &obj = doc.object();

    if(!obj.contains("channels")) {
        qCCritical(CATEGORY_UPDATES) << "No channels data in json file";
        return;
    } else if(!obj["channels"].isArray()) {
        qCCritical(CATEGORY_UPDATES) << "Expected to get an array of channels";
        return;
    }

    const auto &arr = obj["channels"].toArray();

    try {

        for(const auto &val : arr) {
            const Updates::ChannelInfo info(val);

            beginInsertRows(QModelIndex(), m_channels.size(), m_channels.size());
            m_channels.insert(info.name(), info);
            endInsertRows();
        }

    } catch(std::runtime_error &e) {
        qCCritical(CATEGORY_UPDATES) << "Failed to parse update information:" << e.what();
    }
}

const QStringList UpdateRegistry::channelNames() const
{
    auto names = m_channels.keys();

    // Move Development channel to the bottom of the list
    if(names.first().startsWith(QStringLiteral("dev"))) {
        names.move(0, names.size() - 1);
    }

    return names;
}

UpdateRegistry::State UpdateRegistry::state() const
{
    return m_state;
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
    const auto row = index.row();

    if(row >= m_channels.size()) {
        qCDebug(CATEGORY_UPDATES) << "Invalid row index:" << row;
        return QVariant();
    }

    const auto key = channelNames()[row];
    const auto &channel = m_channels[key];

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
        setState(State::ErrorOccured);
        return;
    }

    setState(State::Checking);

    auto *fetcher = new RemoteFileFetcher(this);
    auto *buf = new QBuffer(this);

    fetcher->connect(fetcher, &RemoteFileFetcher::finished, this, [=]() {
        if(fetcher->isError()) {
            qCCritical(CATEGORY_UPDATES).noquote() << "Failed to fetch update information:" << fetcher->errorString();
            setState(State::ErrorOccured);

        } else {
            qCDebug(CATEGORY_UPDATES).noquote() << "Fetched update information from" << m_directoryUrl;
            buf->open(QIODevice::ReadOnly);

            fillFromJson(buf->readAll());
            setState(m_channels.isEmpty() ? State::ErrorOccured : State::Ready);
        }

        fetcher->deleteLater();
        buf->deleteLater();
    });

    if(!fetcher->fetch(m_directoryUrl, buf)) {
        qCCritical(CATEGORY_UPDATES).noquote() << "Failed to fetch update information:" << fetcher->errorString();
        setState(State::ErrorOccured);
        buf->deleteLater();
    }

    m_checkTimer->start(std::chrono::minutes(10));
}

void UpdateRegistry::setState(State newState)
{
    if(m_state == newState) {
        return;
    }

    m_state = newState;
    emit stateChanged();
}

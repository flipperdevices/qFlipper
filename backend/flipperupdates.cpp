#include "flipperupdates.h"

#include <stdexcept>

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>

using namespace Flipper::Updates;

FileInfo::FileInfo():
    m_isValid(false)
{}

FileInfo::FileInfo(const QJsonValue &val):
    FileInfo()
{
    if(!val.isObject()) {
        throw std::runtime_error("Expected FileInfo to be an object");
    }

    const auto &json = val.toObject();
    m_isValid = json.contains("target") && json.contains("type") &&
                              json.contains("url") && json.contains("sha256");

    if(!m_isValid) {
        throw std::runtime_error("Malformed FileInfo");
    }

    m_target = json["target"].toString();
    m_type = json["type"].toString();
    m_url = json["url"].toString();
    m_sha256 = json["sha256"].toString().toLocal8Bit();
}

const QString &FileInfo::target() const
{
    return m_target;
}

const QString &FileInfo::type() const
{
    return m_type;
}

const QString &FileInfo::url() const
{
    return m_url;
}

const QByteArray &FileInfo::sha256() const
{
    return m_sha256;
}

bool FileInfo::isValid() const
{
    return m_isValid;
}

VersionInfo::VersionInfo(const QJsonValue &val)
{
    if(!val.isObject()) {
        throw std::runtime_error("Expected VersionInfo to be an object");
    }

    const auto &json = val.toObject();
    const auto canConstruct = json.contains("version") && json.contains("changelog") &&
                              json.contains("timestamp") && json.contains("files");

    if(!canConstruct) {
        throw std::runtime_error("Malformed VersionInfo");
    } else if(!json["files"].isArray()) {
        throw std::runtime_error("VersionInfo: Expected an array of files");
    } else {}

    m_number = json["version"].toString();
    m_changelog = json["changelog"].toString();
    m_date = QDateTime::fromSecsSinceEpoch(json["timestamp"].toVariant().toULongLong()).date();

    const auto &filesArray = json["files"].toArray();
    for(const auto &file : filesArray) {
        m_files.append(FileInfo(file));
    }
}

const QString &VersionInfo::number() const
{
    return m_number;
}

const QString &VersionInfo::changelog() const
{
    return m_changelog;
}

const QDate &VersionInfo::date() const
{
    return m_date;
}

const QVector<FileInfo> &VersionInfo::files() const
{
    return m_files;
}

const FileInfo VersionInfo::fileInfo(const QString &type, const QString &target) const
{
    const auto it = std::find_if(m_files.cbegin(), m_files.cend(),
        [&](const Updates::FileInfo &arg) {
            return (arg.type() == type) && (target == arg.target());
        });

    return (it != m_files.cend()) ? *it : FileInfo();
}

qint64 VersionInfo::compare(const VersionInfo &other) const
{
    return compare(m_number, other.m_number);
}

qint64 VersionInfo::toNumericValue(const QString &version)
{
    int ret = 0;

    // Get rid of the possible -rcxx suffix
    const auto tokens = version.split('-').first().split('.');
    for(const auto &token : tokens) {
        bool ok; const auto val = token.toInt(&ok);

        if(!ok) return std::numeric_limits<qint64>::min();

        ret *= 1000;
        ret += val;
    }

    return ret;
}

// TODO: Handle -rcxx suffixes correctly
qint64 VersionInfo::compare(const QString &v1, const QString &v2)
{
    return toNumericValue(v1) - toNumericValue(v2);
}

ChannelInfo::ChannelInfo(const QJsonValue &val)
{
    if(!val.isObject()) {
        throw std::runtime_error("Expected ChannelInfo to be an object");
    }

    const auto &json = val.toObject();
    const auto canConstruct = json.contains("id") && json.contains("title") &&
                              json.contains("description") && json.contains("versions");

    if(!canConstruct) {
        throw std::runtime_error("Malformed ChannelInfo");
    } else if(!json["versions"].isArray()) {
        throw std::runtime_error("ChannelInfo: Expected an array of versions");
    } else {}

    m_id = json["id"].toString();
    m_title = json["title"].toString();
    m_description = json["description"].toString();

    const auto &versionArray = json["versions"].toArray();
    for(const auto &version : versionArray) {
        m_versions.append(VersionInfo(version));
    }

    // Json data is not guaranteed to be sorted?
    std::sort(m_versions.begin(), m_versions.end(), [](const VersionInfo &a, const VersionInfo &b) {
        return a.compare(b) > 0;
    });
}

const QString &ChannelInfo::name() const
{
    return m_id;
}

const QString &ChannelInfo::title() const
{
    return m_title;
}

const QString &ChannelInfo::description() const
{
    return m_description;
}

const QVector<VersionInfo> &ChannelInfo::versions() const
{
    return m_versions;
}

const VersionInfo &ChannelInfo::latestVersion() const
{
    return m_versions.first();
}

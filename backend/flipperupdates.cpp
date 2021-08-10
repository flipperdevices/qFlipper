#include "flipperupdates.h"

#include <stdexcept>

#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>

#include "macros.h"

using namespace Flipper::Updates;

FileInfo::FileInfo(const QJsonValue &val)
{
    if(!val.isObject()) {
        throw std::runtime_error("Expected FileInfo to be an object");
    }

    const auto &json = val.toObject();
    const auto canConstruct = json.contains("target") && json.contains("type") &&
                              json.contains("url") && json.contains("sha512");

    if(!canConstruct) {
        throw std::runtime_error("Malformed FileInfo");
    }

    target = json["target"].toString();
    type = json["type"].toString();
    url = json["url"].toString();
    sha256 = json["sha256"].toString().toLocal8Bit();
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

    version = json["version"].toString();
    changelog = json["changelog"].toString();
    timestamp = json["timestamp"].toVariant().toULongLong();

    const auto &filesArray = json["files"].toArray();
    for(const auto &file : filesArray) {
        files.append(file);
    }
}

QString VersionInfo::date() const
{
    return QDateTime::fromSecsSinceEpoch(timestamp).date().toString();
}

const QVector<FileInfo> &VersionInfo::getFiles() const
{
    return files;
}

FileInfo VersionInfo::fileInfo(const QString &type, const QString &target) const
{
    const auto it = std::find_if(files.cbegin(), files.cend(),
        [&](const Updates::FileInfo &arg) {
            return (arg.type == type) && (target == arg.target);
        });

    check_return_val(it != files.cend(), "FileInfo not found", FileInfo());
    return *it;
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

    id = json["id"].toString();
    title = json["title"].toString();
    description = json["description"].toString();

    const auto &versionArray = json["versions"].toArray();
    for(const auto &version : versionArray) {
        versions.append(version);
    }

    // Json data is not guaranteed to be sorted?
    std::sort(versions.begin(), versions.end(), [](const VersionInfo &a, const VersionInfo &b) {
        return a.version > b.version;
    });
}

VersionInfo ChannelInfo::latestVersion() const
{
    return versions.first();
}

QVector<VersionInfo> ChannelInfo::getVersions() const
{
    return versions;
}

VersionInfo ChannelInfo::versionInfo(const QString &versionName) const
{
    const auto it = std::find_if(versions.cbegin(), versions.cend(),
        [&](const Updates::VersionInfo &arg) {
            return versionName == arg.version;
        });

    check_return_val(it != versions.cend(), "VersionInfo not found", VersionInfo());
    return *it;
}

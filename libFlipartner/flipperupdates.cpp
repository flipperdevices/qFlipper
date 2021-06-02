#include "flipperupdates.h"

#include <exception>

#include <QJsonObject>
#include <QJsonArray>

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
    sha512 = json["sha512"].toString().toLocal8Bit();
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

int VersionInfo::indexOf(const QString &target, const QString &type) const
{
    const auto it = std::find_if(files.cbegin(), files.cend(),
        [=](const Updates::FileInfo &arg) {
            return (arg.type == type) && (target == arg.target);
        });

    const auto index = std::distance(files.cbegin(), it);
    if(index < files.size()) {
        return index;
    } else {
        return -1;
    }
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

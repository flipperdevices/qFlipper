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
}

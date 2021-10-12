#include "radiomanifest.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

using namespace Flipper;
using namespace Zero;

RadioManifest::Header::Header():
    m_version(-1),
    m_timestamp(-1)
{}

RadioManifest::Header::Header(const QJsonValue &json)
{
    if(!json.isObject()) {
        throw std::runtime_error("Expected Manifest header to be an object");
    }

    const auto obj = json.toObject();
    if(obj.isEmpty()) {
        throw std::runtime_error("Manifest header is empty");
    }

    if(obj.contains(QStringLiteral("version"))) {
        m_version = obj.value(QStringLiteral("version")).toInt();
    } else {
        throw std::runtime_error("Failed to read manifest version");
    }

    if(obj.contains(QStringLiteral("timestamp"))) {
        m_timestamp = (time_t)obj.value(QStringLiteral("timestamp")).toInt();
    } else {
        throw std::runtime_error("Failed to read manifest timestamp");
    }
}

int RadioManifest::Header::version() const
{
    return m_version;
}

time_t RadioManifest::Header::timestamp() const
{
    return m_timestamp;
}

RadioManifest::Condition::Condition():
    m_type(Type::Unknown)
{}

RadioManifest::Condition::Condition(const QString &text)
{
    if(text.startsWith("==")) {
        m_type = Type::Equals;
        m_version = text.mid(2);

    } else if(text.startsWith('>')) {
        m_type = Type::Greater;
        m_version = text.mid(1);

    } else {
        throw std::runtime_error("Malformed Condition");
    }
}

RadioManifest::Condition::Type RadioManifest::Condition::type() const
{
    return m_type;
}

const QString &RadioManifest::Condition::version() const
{
    return m_version;
}

RadioManifest::FileInfo::FileInfo(const QJsonValue &json)
{
    if(!json.isObject()) {
        throw std::runtime_error("Expected FileInfo to be an object");
    }

    const auto obj = json.toObject();
    if(obj.isEmpty()) {
        throw std::runtime_error("FileInfo is empty");
    }

    const auto canConstruct = obj.contains(QStringLiteral("name")) &&
                              obj.contains(QStringLiteral("sha256")) &&
                              obj.contains(QStringLiteral("address"));
    if(!canConstruct) {
        throw std::runtime_error("Malformed FileInfo");
    }

    m_name = obj.value(QStringLiteral("name")).toString();
    m_sha256 = obj.value(QStringLiteral("sha256")).toVariant().toByteArray();
    m_address = obj.value(QStringLiteral("name")).toVariant().toULongLong();

    if(obj.contains(QStringLiteral("condition"))) {
        m_condition = Condition(obj.value(QStringLiteral("condition")).toString());
    }
}

const QString &RadioManifest::FileInfo::name() const
{
    return m_name;
}

const QByteArray &RadioManifest::FileInfo::sha256() const
{
    return m_sha256;
}

const RadioManifest::Condition &RadioManifest::FileInfo::condition() const
{
    return m_condition;
}

uint32_t RadioManifest::FileInfo::address() const
{
    return m_address;
}

RadioManifest::Section::Section(const QJsonValue &json)
{
    if(!json.isObject()) {
        throw std::runtime_error("Expected Section to be an object");
    }

    const auto obj = json.toObject();
    if(obj.isEmpty()) {
        throw std::runtime_error("Section is empty");
    }

    const auto canConstruct = obj.contains(QStringLiteral("version")) &&
                              obj.contains(QStringLiteral("files"));
    if(!canConstruct) {
        throw std::runtime_error("Malformed Section");
    }

    readVersion(obj.value(QStringLiteral("version")));
    readFiles(obj.value(QStringLiteral("files")));
}

const QString &RadioManifest::Section::version() const
{
    return m_version;
}

const RadioManifest::FileInfoMap &RadioManifest::Section::files() const
{
    return m_files;
}

void RadioManifest::Section::readVersion(const QJsonValue &json)
{
    if(!json.isObject()) {
        throw std::runtime_error("Expected Section version to be an object");
    }

    const auto obj = json.toObject();
    if(obj.isEmpty()) {
        throw std::runtime_error("Section version is empty");
    }

    const auto canConstruct = obj.contains(QStringLiteral("major")) &&
                              obj.contains(QStringLiteral("minor")) &&
                              obj.contains(QStringLiteral("sub"));
    if(!canConstruct) {
        throw std::runtime_error("Malformed section version");
    }

    const auto major = obj.value(QStringLiteral("major")).toInt();
    const auto minor = obj.value(QStringLiteral("minor")).toInt();
    const auto sub = obj.value(QStringLiteral("sub")).toInt();

    m_version = QStringLiteral("%1.%2.%3").arg(major).arg(minor).arg(sub);
}

void RadioManifest::Section::readFiles(const QJsonValue &json)
{
    if(!json.isArray()) {
        throw std::runtime_error("Expected File list to be an array");
    }

    const auto arr = json.toArray();
    if(arr.isEmpty()) {
        throw std::runtime_error("File list is empty");
    }

    for(const auto &value : arr) {
        FileInfo fileInfo(value);
        m_files.insert(fileInfo.name(), fileInfo);
    }
}

RadioManifest::FirmwareInfo::FirmwareInfo(const QJsonValue &json)
{
    if(!json.isObject()) {
        throw std::runtime_error("Expected FirmwareInfo to be an object");
    }

    const auto obj = json.toObject();
    if(obj.isEmpty()) {
        throw std::runtime_error("FirmwareInfo is empty");
    }

    const bool canConstruct = obj.contains(QStringLiteral("fus")) &&
                              obj.contains(QStringLiteral("radio"));
    if(!canConstruct) {
        throw std::runtime_error("Malformed FirmwareInfo");
    }

    m_fus = Section(obj.value(QStringLiteral("fus")));
    m_radio = Section(obj.value(QStringLiteral("radio")));
}

const RadioManifest::Section &RadioManifest::FirmwareInfo::fus() const
{
    return m_fus;
}

const RadioManifest::Section &RadioManifest::FirmwareInfo::radio() const
{
    return m_radio;
}

RadioManifest::RadioManifest(const QByteArray &text)
{
    if(text.isEmpty()) {
        setError("JSON text is empty");
        return;
    }

    const auto doc = QJsonDocument::fromJson(text);

    if(!doc.isObject()) {
        setError("Expected RadioManifest to be a JSON object");
        return;
    }

    const auto obj = doc.object();
    const auto canConstruct = obj.contains(QStringLiteral("manifest")) &&
                              obj.contains(QStringLiteral("copro"));
    if(!canConstruct) {
        setError("Malformed RadioManifest");
        return;
    }

    try {
        m_header = Header(obj.value(QStringLiteral("manifest")));
        m_firmware = FirmwareInfo(obj.value(QStringLiteral("copro")));

    } catch(const std::runtime_error &e) {
        setError(e.what());
    }
}

const RadioManifest::Header &RadioManifest::header() const
{
    return m_header;
}

const RadioManifest::FirmwareInfo &RadioManifest::firmware() const
{
    return m_firmware;
}

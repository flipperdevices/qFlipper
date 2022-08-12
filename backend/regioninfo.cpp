#include "regioninfo.h"

#include <QLoggingCategory>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

Q_LOGGING_CATEGORY(LOG_REGION_INFO, "RGN")

RegionInfo::RegionInfo(const QByteArray &text):
    m_isValid(false)
{
    const auto doc = QJsonDocument::fromJson(text);

    if(doc.isNull()) {
        qCCritical(LOG_REGION_INFO) << "Failed to parse the document";
        return;
    } else if(!doc.isObject()) {
        qCCritical(LOG_REGION_INFO) << "Json document is not an object";
        return;
    }

    static const auto errorKey = QStringLiteral("error");
    static const auto successKey = QStringLiteral("success");

    const auto obj = doc.object();
    if(obj.contains(errorKey)) {
        parseError(obj.value(errorKey));
    } else if(obj.contains(successKey)) {
        parseSuccess(obj.value(successKey));
    } else {
        qCCritical(LOG_REGION_INFO) << "Got valid JSON object, but neither success nor error info";
    }
}

bool RegionInfo::isValid() const
{
    return m_isValid;
}

bool RegionInfo::isError() const
{
    return m_isError;
}

const QString &RegionInfo::erroString() const
{
    return m_errorString;
}

int RegionInfo::errorCode() const
{
    return m_errorCode;
}

const RegionInfo::CountryKey &RegionInfo::detectedCountry() const
{
    return m_country;
}

const RegionInfo::BandKeyList RegionInfo::countryBandKeys(const CountryKey &key) const
{
    return m_countries[key];
}

const RegionInfo::BandList RegionInfo::bandsByKeys(const BandKeyList &keys) const
{
    Q_UNUSED(keys)
    BandList ret;
    // TODO Not implemented yet
    return ret;
}

const RegionInfo::BandList RegionInfo::bandsByCountry(const CountryKey &key) const
{
    Q_UNUSED(key)
    BandList ret;
    // TODO Not implemented yet
    return ret;
}

void RegionInfo::parseError(const QJsonValue &val)
{
    if(!val.isObject()) {
        qCCritical(LOG_REGION_INFO) << "Error info is not an object";
        return;
    }

    const auto obj = val.toObject();

    static const auto codeKey = QStringLiteral("code");
    static const auto textKey = QStringLiteral("text");

    const auto isComplete = obj.contains(codeKey) && obj.contains(textKey);
    if(!isComplete) {
        qCCritical(LOG_REGION_INFO) << "Error object is missing one or more fields";
        return;
    }

    m_errorCode = obj[codeKey].toInt();
    m_errorString = obj[textKey].toString();

    m_isValid = true;
    m_isError = true;
}

void RegionInfo::parseSuccess(const QJsonValue &val)
{
    if(!val.isObject()) {
        qCCritical(LOG_REGION_INFO) << "Success info is not an object";
        return;
    }

    const auto obj = val.toObject();

    const auto bandsKey = QStringLiteral("bands");
    const auto countriesKey = QStringLiteral("countries");
    const auto countryKey = QStringLiteral("country");
    const auto defaultKey = QStringLiteral("default");

    const auto isComplete = obj.contains(bandsKey) && obj.contains(countriesKey) &&
                            obj.contains(countryKey) && obj.contains(defaultKey);
    if(!isComplete) {
        qCCritical(LOG_REGION_INFO) << "Success object is missing one or more fields";
        return;
    }

    m_isError = false;
    m_isValid = parseBands(obj[bandsKey]) && parseCountries(obj[countriesKey]) &&
            parseCountry(obj[countryKey]) && parseDefault(obj[defaultKey]);
}

bool RegionInfo::parseBands(const QJsonValue &val)
{
    if(!val.isObject()) {
        qCCritical(LOG_REGION_INFO) << "Bands info is not an object";
        return false;
    }

    const auto obj = val.toObject();

    if(obj.isEmpty()) {
        qCCritical(LOG_REGION_INFO) << "Bands info is an empty object";
        return false;
    }

    const auto keys = obj.keys();
    for(const auto &key : keys) {
        if(!insertBand(key.toLocal8Bit(), obj[key])) {
            return false;
        }
    }

    return true;
}

bool RegionInfo::parseCountries(const QJsonValue &val)
{
    if(!val.isObject()) {
        qCCritical(LOG_REGION_INFO) << "Countries info is not an object";
        return false;
    }

    const auto obj = val.toObject();

    if(obj.isEmpty()) {
        qCCritical(LOG_REGION_INFO) << "Countries info is an empty object";
        return false;
    }

    const auto keys = obj.keys();
    for(const auto &key : keys) {
        if(!insertCountry(key.toLocal8Bit(), obj[key])) {
            return false;
        }
    }

    return true;
}

bool RegionInfo::parseCountry(const QJsonValue &val)
{
    if(!val.isString()) {
        qCCritical(LOG_REGION_INFO) << "Country is not a string";
        return false;

    } else {
        m_country = val.toString().toLocal8Bit();
        return true;
    }
}

bool RegionInfo::parseDefault(const QJsonValue &val)
{
    if(!val.isArray()) {
        qCCritical(LOG_REGION_INFO) << "Default bands list is not an array";
        return false;
    }

    const QJsonArray arr = val.toArray();

    if(arr.isEmpty()) {
        qCCritical(LOG_REGION_INFO) << "Default bands is an empty array";
        return false;
    }

    for(const auto &el : arr) {
        m_defaultBandKeys.append(el.toString().toLocal8Bit());
    }

    return true;
}

bool RegionInfo::insertBand(const BandKey &key, const QJsonValue &val)
{
    if(!val.isObject()) {
        qCCritical(LOG_REGION_INFO) << "Band info is not an object";
        return false;
    }

    const auto obj = val.toObject();

    const auto startKey = QStringLiteral("start");
    const auto endKey = QStringLiteral("end");
    const auto dutyCycleKey = QStringLiteral("duty_cycle");
    const auto maxPowerKey = QStringLiteral("max_power");

    const auto isComplete = obj.contains(startKey) && obj.contains(endKey) &&
                            obj.contains(dutyCycleKey) && obj.contains(maxPowerKey);

    if(!isComplete) {
        qCCritical(LOG_REGION_INFO) << "Band object is missing one or more fields";
        return false;
    }

    const Band band = {
        (unsigned)obj[startKey].toInt(),
        (unsigned)obj[endKey].toInt(),
        obj[maxPowerKey].toInt(),
        (unsigned)obj[dutyCycleKey].toInt(),
    };

    m_bands.insert(key, band);

    return true;
}

bool RegionInfo::insertCountry(const CountryKey &key, const QJsonValue &val)
{
    if(!val.isArray()) {
        qCCritical(LOG_REGION_INFO) << "Country bands list is not an array";
        return false;
    }

    const QJsonArray arr = val.toArray();

    if(arr.isEmpty()) {
        qCCritical(LOG_REGION_INFO) << "Country bands is an empty array";
        return false;
    }

    BandKeyList bandKeys;

    for(const auto &el : arr) {
        bandKeys.append(el.toString().toLocal8Bit());
    }

    m_countries.insert(key, bandKeys);

    return true;
}

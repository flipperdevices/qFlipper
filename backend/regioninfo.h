#pragma once

#include <QHash>
#include <QVector>
#include <QByteArray>

class QJsonValue;

class RegionInfo
{
public:
    struct Band {
        uint32_t start;
        uint32_t end;
        int32_t powerLimit;
        uint32_t dutyCycle;
    };

    using BandList = QList<Band>;
    using CountryKey = QByteArray;
    using BandKey = QByteArray;
    using BandKeyList = QList<BandKey>;

    RegionInfo();
    RegionInfo(const QByteArray &text);

    bool isValid() const;
    bool isError() const;

    bool hasCountryCode() const;

    const QString &errorString() const;
    int errorCode() const;

    const CountryKey &detectedCountry() const;
    const BandKeyList &defaultBandKeys() const;
    const BandKeyList countryBandKeys(const CountryKey &key) const;

    const BandList bandsByKeys(const BandKeyList &keys) const;
    const BandList bandsByCountry(const CountryKey &key) const;

private:
    void parseError(const QJsonValue &val);
    void parseSuccess(const QJsonValue &val);

    bool parseBands(const QJsonValue &val);
    bool parseCountries(const QJsonValue &val);
    bool parseCountry(const QJsonValue &val);
    bool parseDefault(const QJsonValue &val);

    bool insertBand(const BandKey &key, const QJsonValue &val);
    bool insertCountry(const CountryKey &key, const QJsonValue &val);

    CountryKey m_country;
    BandKeyList m_defaultBandKeys;
    QHash<CountryKey, BandKeyList> m_countries;
    QHash<BandKey, Band> m_bands;

    bool m_isValid;
    bool m_isError;
    QString m_errorString;
    int m_errorCode;
};


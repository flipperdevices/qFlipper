#pragma once

#include <QVector>
#include <QByteArray>

namespace Flipper {
namespace Zero {

class FactoryInfo
{
public:
    enum class Color {
        Unknown = 0x00,
        Black,
        White
    };

    enum class Region {
        Dev = 0x00,
        EuRu,
        USCaAu,
        JpPlus,
        Worldwide
    };

    FactoryInfo(const QByteArray &data);

    static qint64 size();

    bool isValid() const;

    uint8_t format() const;
    uint8_t version() const;
    uint8_t target() const;
    uint8_t body() const;
    uint8_t connect() const;
    time_t date() const;

    const QString &name() const;

    Color color() const;
    Region region() const;

private:
    static size_t strnLen(const char *str, size_t maxlen);

    void parseV0(const QByteArray &data);
    void parseV1(const QByteArray &data);
    void parseV2(const QByteArray &data);

    bool m_isValid;

    uint8_t m_format;
    uint8_t m_version;
    uint8_t m_target;
    uint8_t m_body;
    uint8_t m_connect;
    time_t m_date;
    QString m_name;

    Color m_color;
    Region m_region;
};

}
}

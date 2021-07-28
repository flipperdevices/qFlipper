#include "factoryinfo.h"

#include <QRegExp>
#include "macros.h"

#define FACTORYINFO_SIZE 24
#define FACTORYINFO_NAME_SIZE 8
#define FACTORYINFO_V1_MAGIC 0xbabe

namespace Flipper {
namespace Zero {

#pragma pack(push,1)

struct OTPV0 {
    const uint8_t version;
    const uint8_t target;
    const uint8_t body;
    const uint8_t connect;
    const uint32_t timestamp;

    const char name[FACTORYINFO_NAME_SIZE];
};

struct OTPV1 {
    const uint16_t magic;
    const uint8_t format;
    const uint8_t reserved1;
    const uint32_t timestamp;

    const uint8_t version;
    const uint8_t target;
    const uint8_t body;
    const uint8_t connect;
    const uint8_t color;
    const uint8_t region;
    const uint16_t reserved2;

    const char name[FACTORYINFO_NAME_SIZE];
};

#pragma pack(pop)

static_assert(sizeof(OTPV0) == 16, "Check struct packing for OTPV0");
static_assert(sizeof(OTPV1) == 24, "Check struct packing for OTPV1");

FactoryInfo::FactoryInfo(const QByteArray &data):
    m_isValid(false),
    m_format(0),
    m_color(Color::Unknown),
    m_region(Region::Unknown)
{
    check_return_void(data.size() == FACTORYINFO_SIZE, "Bad data size");
    check_return_void(data != QByteArray(FACTORYINFO_SIZE, 0xff), "Data seems to be unprogrammed");

    if(*((uint16_t*)data.data()) == FACTORYINFO_V1_MAGIC) {
        parseV1(data);
    } else {
        parseV0(data);
    }

    QRegExp ascii("[^A-Za-z0-9.]");
    check_return_void(ascii.indexIn(m_name) < 0, "Illegal character in the device name");

    m_isValid = true;
}

qint64 FactoryInfo::size()
{
    return FACTORYINFO_SIZE;
}

bool FactoryInfo::isValid() const
{
    return m_isValid;
}

uint8_t FactoryInfo::format() const
{
    return m_format;
}

uint8_t FactoryInfo::version() const
{
    return m_version;
}

uint8_t FactoryInfo::target() const
{
    return m_target;
}

uint8_t FactoryInfo::body() const
{
    return m_body;
}

uint8_t FactoryInfo::connect() const
{
    return m_connect;
}

const QString &FactoryInfo::name() const
{
    return m_name;
}

FactoryInfo::Color FactoryInfo::color() const
{
    return m_color;
}

FactoryInfo::Region FactoryInfo::region() const
{
    return m_region;
}

size_t FactoryInfo::strnLen(const char *str, size_t maxlen)
{
    for(size_t i = 0; i < maxlen; ++i) {
        if(!str[i]) return i;
    }

    return maxlen;
}

void FactoryInfo::parseV0(const QByteArray &data)
{
    const auto *otp = (OTPV0*)(data.data());

    m_version = otp->version;
    m_target = otp->target;
    m_body = otp->body;
    m_connect = otp->connect;
    m_date = otp->timestamp;
    m_name = QString::fromLatin1(otp->name, strnLen(otp->name, FACTORYINFO_NAME_SIZE));
}

void FactoryInfo::parseV1(const QByteArray &data)
{
    const auto *otp = (OTPV1*)(data.data());

    m_version = otp->version;
    m_target = otp->target;
    m_body = otp->body;
    m_connect = otp->connect;
    m_date = otp->timestamp;
    m_name = QString::fromLatin1(otp->name, strnLen(otp->name, FACTORYINFO_NAME_SIZE));

    m_format = otp->format;
    m_color = (Color)otp->color;
    m_region = (Region)otp->region;
}

time_t FactoryInfo::date() const
{
    return m_date;
}

}}

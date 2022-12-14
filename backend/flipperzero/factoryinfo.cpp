#include "factoryinfo.h"

#include <QRegExp>
#include "debug.h"

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

struct OTPHeader {
    const uint16_t magic;
    const uint8_t format;
    const uint8_t reserved;
    const uint32_t timestamp;
};

struct OTPV1 {
    const OTPHeader header;

    const uint8_t version;
    const uint8_t target;
    const uint8_t body;
    const uint8_t connect;
    const uint8_t color;
    const uint8_t region;
    const uint16_t reserved;

    const char name[FACTORYINFO_NAME_SIZE];
};

struct OTPV2 {
    const OTPHeader header;

    const uint8_t version;
    const uint8_t target;
    const uint8_t body;
    const uint8_t connect;
    const uint8_t display;
    const uint8_t reserved1;
    const uint16_t reserved2;

    const uint8_t color;
    const uint8_t region;
    const uint16_t reserved3;
    const uint32_t reserved4;

    const char name[FACTORYINFO_NAME_SIZE];
};

#pragma pack(pop)

static_assert(sizeof(OTPV0) == 16, "Check struct packing for OTPv0");
static_assert(sizeof(OTPV1) == 24, "Check struct packing for OTPv1");
static_assert(sizeof(OTPV2) == 32, "Check struct packing for OTPv2");

#define FACTORYINFO_SIZE (sizeof(OTPV2))

FactoryInfo::FactoryInfo(const QByteArray &data):
    m_isValid(false),
    m_format(0),
    m_color(Color::Unknown),
    m_region(Region::Dev)
{
    check_return_void(data.size() == FACTORYINFO_SIZE, "Bad data size");
    check_return_void(data != QByteArray(FACTORYINFO_SIZE, '\xff'), "Data seems to be unprogrammed");

    auto *header = (OTPHeader*)(data.data());

    if(header->magic == FACTORYINFO_V1_MAGIC) {

        if(header->format == 1) {
            parseV1(data);
        } else if(header->format == 2) {
            parseV2(data);
        } else {
            error_msg("Unsupported OTP version");
            return;
        }

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

Color FactoryInfo::color() const
{
    return m_color;
}

Region FactoryInfo::region() const
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
    m_name = QString::fromLatin1(otp->name, (int)strnLen(otp->name, FACTORYINFO_NAME_SIZE));
}

void FactoryInfo::parseV1(const QByteArray &data)
{
    const auto *otp = (OTPV1*)(data.data());

    m_version = otp->version;
    m_target = otp->target;
    m_body = otp->body;
    m_connect = otp->connect;
    m_date = otp->header.timestamp;
    m_name = QString::fromLatin1(otp->name, (int)strnLen(otp->name, FACTORYINFO_NAME_SIZE));

    m_format = otp->header.format;
    m_color = (Color)otp->color;
    m_region = (Region)otp->region;
}

void FactoryInfo::parseV2(const QByteArray &data)
{
    const auto *otp = (OTPV2*)(data.data());

    m_version = otp->version;
    m_target = otp->target;
    m_body = otp->body;
    m_connect = otp->connect;
    m_date = otp->header.timestamp;
    m_name = QString::fromLatin1(otp->name, (int)strnLen(otp->name, FACTORYINFO_NAME_SIZE));

    m_format = otp->header.format;
    m_color = (Color)otp->color;
    m_region = (Region)otp->region;
}

qint64 FactoryInfo::date() const
{
    return m_date;
}

}}

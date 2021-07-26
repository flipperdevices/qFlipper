#include "factoryinfo.h"

#include <QRegExp>
#include "macros.h"

#define FACTORYINFO_SIZE 24
#define FACTORYINFO_NAME_SIZE 8
#define FACTORYINFO_V1_MAGIC 0xbabe

namespace Flipper {
namespace Zero {

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

    QRegExp ascii("[^A-Za-z0-9]");
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

void FactoryInfo::parseV0(const QByteArray &data)
{
    m_version = data[0];
    m_target = data[1];
    m_body = data[2];
    m_connect = data[3];
    m_date = *((time_t*)data.mid(4, sizeof(time_t)).data()),
    m_name = data.mid(8, FACTORYINFO_NAME_SIZE);
}

void FactoryInfo::parseV1(const QByteArray &data)
{
    m_format = data[2];
    m_date = *((time_t*)data.mid(4, sizeof(time_t)).data()),
    m_version = data[8];
    m_target = data[9];
    m_body = data[10];
    m_connect = data[11];
    m_color = (Color)data[12];
    m_region = (Region)data[13];
    m_name = data.mid(16, FACTORYINFO_NAME_SIZE);
}

time_t FactoryInfo::date() const
{
    return m_date;
}

}}

#include "factoryinfo.h"
#include "macros.h"

#define FACTORYINFO_SIZE 16

namespace Flipper {
namespace Zero {

FactoryInfo::FactoryInfo(const QByteArray &data):
    m_isValid(data.size() >= FACTORYINFO_SIZE)
{
    m_version = data[0];
    m_target = data[1];
    m_body = data[2];
    m_connect = data[3];
    m_date = *((time_t*)data.mid(4,7).data()),
    m_name = data.mid(8, 8);
}

qint64 FactoryInfo::size()
{
    return FACTORYINFO_SIZE;
}

bool FactoryInfo::isValid() const
{
    return m_isValid;
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

time_t FactoryInfo::date() const
{
    return m_date;
}

}}

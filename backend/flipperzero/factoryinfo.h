#pragma once

#include <QVector>
#include <QByteArray>

namespace Flipper {
namespace Zero {

class FactoryInfo
{
public:
    FactoryInfo(const QByteArray &data);

    static qint64 size();

    bool isValid() const;

    uint8_t version() const;
    uint8_t target() const;
    uint8_t body() const;
    uint8_t connect() const;
    time_t date() const;
    const QString &name() const;

private:
    bool m_isValid;
    uint8_t m_version;
    uint8_t m_target;
    uint8_t m_body;
    uint8_t m_connect;
    time_t m_date;
    QString m_name;
};

}
}

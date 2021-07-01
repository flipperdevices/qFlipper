#pragma once

#include <QString>
#include <QByteArray>

namespace STM32WB55 {

class OptionBytes
{
    OptionBytes();

public:
    OptionBytes(const QByteArray &data);

    static OptionBytes invalid();
    static qint64 size();

    const QByteArray &data() const;
    bool isValid() const;

    bool nBoot0() const;
    bool nBoot1() const;
    bool nSwBoot0() const;

    void setNBoot0(bool set);
    void setNBoot1(bool set);
    void setNSwBoot0(bool set);

private:
    bool getBit(size_t addr, size_t pos) const;
    void setBit(size_t addr, size_t pos, bool set);

    QByteArray m_data;
    bool m_isValid;
};

}

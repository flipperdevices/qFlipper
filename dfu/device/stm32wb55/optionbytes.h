#pragma once

#include <QString>
#include <QByteArray>

namespace STM32 {
namespace WB55 {

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

    uint8_t SFSA() const;

    void setNBoot0(bool set);
    void setNBoot1(bool set);
    void setNSwBoot0(bool set);

private:
    bool getBit(size_t addr, size_t pos) const;
    void setBit(size_t addr, size_t pos, bool set);

    uint8_t getByte(size_t addr, size_t pos) const;

    QByteArray m_data;
    bool m_isValid;
};

}
}

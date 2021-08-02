#include "optionbytes.h"
#include "macros.h"

#define OPTION_BYTES_SIZE 128

#define NORMAL 0
#define COMPLEMENT 1

namespace STM32 {
namespace WB55 {

OptionBytes::OptionBytes():
    m_isValid(false)
{}

OptionBytes::OptionBytes(const QByteArray &data):
    m_isValid(data.size() == OPTION_BYTES_SIZE),
    m_data(*(OptionBytesData*)(data.data()))
{}

OptionBytes OptionBytes::invalid()
{
    return OptionBytes();
}

qint64 OptionBytes::size()
{
    return OPTION_BYTES_SIZE;
}

bool OptionBytes::isValid() const
{
    return m_isValid;
}

QByteArray OptionBytes::data() const
{
    return QByteArray::fromRawData((const char*)(&m_data), sizeof(OptionBytesData));
}

bool OptionBytes::nBoot0() const
{
    return m_data.word1[NORMAL].nBoot0;
}

bool OptionBytes::nBoot1() const
{
    return m_data.word1[NORMAL].nBoot1;
}

bool OptionBytes::nSwBoot0() const
{
    return m_data.word1[NORMAL].nSwBoot0;
}

bool OptionBytes::SRAM2PE() const
{
    return m_data.word1[NORMAL].SRAM2PE;
}

bool OptionBytes::SRAM2RST() const
{
    return m_data.word1[NORMAL].SRAM2RST;
}

uint8_t OptionBytes::AGCTRIM() const
{
    return m_data.word1[NORMAL].AGCTRIM;
}

bool OptionBytes::FSD() const
{
    return m_data.word9[NORMAL].FSD;
}

bool OptionBytes::DDS() const
{
    return m_data.word9[NORMAL].FSD;
}

bool OptionBytes::BRSD() const
{
    return m_data.word10[NORMAL].BRSD;
}

bool OptionBytes::NBRSD() const
{
    return m_data.word10[NORMAL].NBRSD;
}

bool OptionBytes::C2OPT() const
{
    return m_data.word10[NORMAL].C2OPT;
}

bool OptionBytes::PCROPRDP() const
{
    return m_data.word3[NORMAL].PCROPRDP;
}

uint8_t OptionBytes::RDP() const
{
    return m_data.word1[NORMAL].RDP;
}

bool OptionBytes::ESE() const
{
    return m_data.word1[NORMAL].ESE;
}

uint8_t OptionBytes::BORLEV() const
{
    return m_data.word1[NORMAL].BORLEV;
}

bool OptionBytes::nRSTSTOP() const
{
    return m_data.word1[NORMAL].nRSTSTOP;
}

bool OptionBytes::nRSTSTDBY() const
{
    return m_data.word1[NORMAL].nRSTSTDBY;
}

bool OptionBytes::nRSTSHDW() const
{
    return m_data.word1[NORMAL].nRSTSHDW;
}

bool OptionBytes::IWDGSW() const
{
    return m_data.word1[NORMAL].IWDGSW;
}

bool OptionBytes::IWDGSTOP() const
{
    return m_data.word1[NORMAL].IWDGSTOP;
}

bool OptionBytes::IWGDSTDBY() const
{
    return m_data.word1[NORMAL].IWGDSTDBY;
}

bool OptionBytes::WWDGSW() const
{
    return m_data.word1[NORMAL].WWDGSW;
}

uint8_t OptionBytes::SFSA() const
{
    return m_data.word9[NORMAL].SFSA;
}

uint8_t OptionBytes::SBRSA() const
{
    return m_data.word10[NORMAL].SBRSA;
}

uint8_t OptionBytes::SNBRSA() const
{
    return m_data.word10[NORMAL].SNBRSA;
}

uint16_t OptionBytes::PCROP1ASTRT() const
{
    return m_data.word2[NORMAL].PCROP1ASTRT;
}

uint16_t OptionBytes::PCROP1AEND() const
{
    return m_data.word3[NORMAL].PCROP1AEND;
}

uint16_t OptionBytes::PCROP1BSTRT() const
{
    return m_data.word6[NORMAL].PCROP1BSTRT;
}

uint16_t OptionBytes::PCROP1BEND() const
{
    return m_data.word7[NORMAL].PCROP1BEND;
}

uint8_t OptionBytes::WRP1ASTRT() const
{
    return m_data.word4[NORMAL].WRP1ASTRT;
}

uint8_t OptionBytes::WRP1AEND() const
{
    return m_data.word4[NORMAL].WRP1AEND;
}

uint8_t OptionBytes::WRP1BSTRT() const
{
    return m_data.word5[NORMAL].WRP1BSTRT;
}

uint8_t OptionBytes::WRP1BEND() const
{
    return m_data.word5[NORMAL].WRP1BEND;
}

uint16_t OptionBytes::IPCCDBA() const
{
    return m_data.word8[NORMAL].IPCCDBA;
}

uint32_t OptionBytes::SBRV() const
{
    return m_data.word10[NORMAL].SBRV;
}

void OptionBytes::setNBoot0(bool set)
{
    m_data.word1[NORMAL].nBoot0 = set;
    m_data.word1[COMPLEMENT].nBoot0 = !set;
}

void OptionBytes::setNBoot1(bool set)
{
    m_data.word1[NORMAL].nBoot1 = set;
    m_data.word1[COMPLEMENT].nBoot1 = !set;
}

void OptionBytes::setNSwBoot0(bool set)
{
    m_data.word1[NORMAL].nSwBoot0 = set;
    m_data.word1[COMPLEMENT].nSwBoot0 = !set;
}

void OptionBytes::setBORLEV(uint8_t val)
{
    m_data.word1[NORMAL].nSwBoot0 = val;
    m_data.word1[COMPLEMENT].nSwBoot0 = ~val;
}

void OptionBytes::setnRSTSTOP(bool set)
{
    m_data.word1[NORMAL].nRSTSTOP = set;
    m_data.word1[COMPLEMENT].nRSTSTOP = !set;
}

void OptionBytes::setnRSTSTDBY(bool set)
{
    m_data.word1[NORMAL].nRSTSTDBY = set;
    m_data.word1[COMPLEMENT].nRSTSTDBY = !set;
}

void OptionBytes::setnRSTSHDW(bool set)
{
    m_data.word1[NORMAL].nRSTSHDW = set;
    m_data.word1[COMPLEMENT].nRSTSHDW = !set;
}

void OptionBytes::setIWDGSW(bool set)
{
    m_data.word1[NORMAL].IWDGSW = set;
    m_data.word1[COMPLEMENT].IWDGSW = !set;
}

void OptionBytes::setIWDGSTOP(bool set)
{
    m_data.word1[NORMAL].IWDGSTOP = set;
    m_data.word1[COMPLEMENT].IWDGSTOP = !set;
}

void OptionBytes::setIWGDSTDBY(bool set)
{
    m_data.word1[NORMAL].IWGDSTDBY = set;
    m_data.word1[COMPLEMENT].IWGDSTDBY = !set;
}

void OptionBytes::setWWDGSW(bool set)
{
    m_data.word1[NORMAL].WWDGSW = set;
    m_data.word1[COMPLEMENT].WWDGSW = !set;
}

}
}

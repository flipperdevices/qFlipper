#include "optionbytes.h"
#include "macros.h"

#define OPTION_BYTES_SIZE 128

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
    return m_data.word1[0].nBoot0;
}

bool OptionBytes::nBoot1() const
{
    return m_data.word1[0].nBoot1;
}

bool OptionBytes::nSwBoot0() const
{
    return m_data.word1[0].nSwBoot0;
}

bool OptionBytes::SRAM2PE() const
{
    return m_data.word1[0].SRAM2PE;
}

bool OptionBytes::SRAM2RST() const
{
    return m_data.word1[0].SRAM2RST;
}

uint8_t OptionBytes::AGCTRIM() const
{
    return m_data.word1[0].AGCTRIM;
}

bool OptionBytes::FSD() const
{
    return m_data.word9[0].FSD;
}

bool OptionBytes::DDS() const
{
    return m_data.word9[0].FSD;
}

bool OptionBytes::BRSD() const
{
    return m_data.word10[0].BRSD;
}

bool OptionBytes::NBRSD() const
{
    return m_data.word10[0].NBRSD;
}

bool OptionBytes::C2OPT() const
{
    return m_data.word10[0].C2OPT;
}

bool OptionBytes::PCROPRDP() const
{
    return m_data.word3[0].PCROPRDP;
}

uint8_t OptionBytes::RDP() const
{
    return m_data.word1[0].RDP;
}

bool OptionBytes::ESE() const
{
    return m_data.word1[0].ESE;
}

uint8_t OptionBytes::BORLEV() const
{
    return m_data.word1[0].BORLEV;
}

bool OptionBytes::nRSTSTOP() const
{
    return m_data.word1[0].nRSTSTOP;
}

bool OptionBytes::nRSTSTDBY() const
{
    return m_data.word1[0].nRSTSTDBY;
}

bool OptionBytes::nRSTSHDW() const
{
    return m_data.word1[0].nRSTSHDW;
}

bool OptionBytes::IWDGSW() const
{
    return m_data.word1[0].IWDGSW;
}

bool OptionBytes::IWDGSTOP() const
{
    return m_data.word1[0].IWDGSTOP;
}

bool OptionBytes::IWGDSTDBY() const
{
    return m_data.word1[0].IWGDSTDBY;
}

bool OptionBytes::WWDGSW() const
{
    return m_data.word1[0].WWDGSW;
}

uint8_t OptionBytes::SFSA() const
{
    return m_data.word9[0].SFSA;
}

uint8_t OptionBytes::SBRSA() const
{
    return m_data.word10[0].SBRSA;
}

uint8_t OptionBytes::SNBRSA() const
{
    return m_data.word10[0].SNBRSA;
}

uint16_t OptionBytes::PCROP1ASTRT() const
{
    return m_data.word2[0].PCROP1ASTRT;
}

uint16_t OptionBytes::PCROP1AEND() const
{
    return m_data.word3[0].PCROP1AEND;
}

uint16_t OptionBytes::PCROP1BSTRT() const
{
    return m_data.word6[0].PCROP1BSTRT;
}

uint16_t OptionBytes::PCROP1BEND() const
{
    return m_data.word7[0].PCROP1BEND;
}

uint8_t OptionBytes::WRP1ASTRT() const
{
    return m_data.word4[0].WRP1ASTRT;
}

uint8_t OptionBytes::WRP1AEND() const
{
    return m_data.word4[0].WRP1AEND;
}

uint8_t OptionBytes::WRP1BSTRT() const
{
    return m_data.word5[0].WRP1BSTRT;
}

uint8_t OptionBytes::WRP1BEND() const
{
    return m_data.word5[0].WRP1BEND;
}

uint16_t OptionBytes::IPCCDBA() const
{
    return m_data.word8[0].IPCCDBA;
}

uint32_t OptionBytes::SBRV() const
{
    return m_data.word10[0].SBRV;
}

void OptionBytes::setNBoot0(bool set)
{
    m_data.word1[0].nBoot0 = set;
    m_data.word1[1].nBoot0 = !set;
}

void OptionBytes::setNBoot1(bool set)
{
    m_data.word1[0].nBoot1 = set;
    m_data.word1[1].nBoot1 = !set;
}

void OptionBytes::setNSwBoot0(bool set)
{
    m_data.word1[0].nSwBoot0 = set;
    m_data.word1[1].nSwBoot0 = !set;
}

void OptionBytes::setBORLEV(uint8_t val)
{
    m_data.word1[0].nSwBoot0 = val;
    m_data.word1[1].nSwBoot0 = ~val;
}

}
}

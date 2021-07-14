#include "optionbytes.h"
#include "macros.h"

#define OPTION_BYTES_SIZE 128

#define nBOOT0_ADDR 0
#define nBOOT0_POS 27

#define nBOOT1_ADDR 0
#define nBOOT1_POS 23

#define nSWBOOT0_ADDR 0
#define nSWBOOT0_POS 26

#define SFSA_ADDR 0x70
#define SFSA_POS 0

namespace STM32 {
namespace WB55 {

OptionBytes::OptionBytes():
    m_data(OPTION_BYTES_SIZE, 0),
    m_isValid(false)
{}

OptionBytes::OptionBytes(const QByteArray &data):
    m_data(data),
    m_isValid(data.size() == OPTION_BYTES_SIZE)
{}

OptionBytes OptionBytes::invalid()
{
    return OptionBytes();
}

qint64 OptionBytes::size()
{
    return OPTION_BYTES_SIZE;
}

const QByteArray &OptionBytes::data() const
{
    return m_data;
}

bool OptionBytes::isValid() const
{
    return m_isValid;
}

bool OptionBytes::nBoot0() const
{
    return getBit(nBOOT0_ADDR, nBOOT0_POS);
}

bool OptionBytes::nBoot1() const
{
    return getBit(nBOOT1_ADDR, nBOOT1_POS);
}

bool OptionBytes::nSwBoot0() const
{
    return getBit(nSWBOOT0_ADDR, nSWBOOT0_POS);
}

uint8_t OptionBytes::SFSA() const
{
    return getByte(SFSA_ADDR, SFSA_POS);
}

void OptionBytes::setNBoot0(bool set)
{
    setBit(nBOOT0_ADDR, nBOOT0_POS, set);
}

void OptionBytes::setNBoot1(bool set)
{
    setBit(nBOOT1_ADDR, nBOOT1_POS, set);
}

void OptionBytes::setNSwBoot0(bool set)
{
    setBit(nSWBOOT0_ADDR, nSWBOOT0_POS, set);
}

bool OptionBytes::getBit(size_t addr, size_t pos) const
{
    const uint idx = addr + pos / 8;
    const uint cidx = idx + sizeof(uint32_t);
    const char mask = 1U << (pos % 8);

    const auto bit = (m_data[idx] & mask) != 0;
    const auto cbit = (m_data[cidx] & mask) != 0;

    check_continue(bit == !cbit, "Option bytes complement mismatch");
    return bit;
}

void OptionBytes::setBit(size_t addr, size_t pos, bool set)
{
    const uint idx = addr + pos / 8;
    const uint cidx = idx + sizeof(uint32_t);
    const char mask = 1U << (pos % 8);

    char byte, cbyte;

    if(set) {
        byte = m_data[idx] | mask;
        cbyte = m_data[cidx] & (~mask);
    } else {
        byte = m_data[idx] & (~mask);
        cbyte = m_data[cidx] | mask;
    }

    m_data[idx] = byte;
    m_data[cidx] = cbyte;
}

uint8_t OptionBytes::getByte(size_t addr, size_t pos) const
{
    const uint idx = addr + pos;
    const uint cidx = idx + sizeof(uint32_t);

    const uint8_t byte = m_data[idx];
    const uint8_t cbyte = m_data[cidx];

    check_continue((byte | cbyte) == 0xFF, "Option bytes mismatch");
    return byte;
}

}
}

#include "optionbytes.h"

#include <QRegExp>
#include <QIODevice>

#include "macros.h"

// TODO: Try a dictionary-based implementation instead

#define OPTION_BYTES_SIZE 128

#define NORMAL 0
#define COMPLEMENT 1

#define toHexStr(num) (QString::number(num, 16))
#define check_register(equal, reg, msg) \
    if(reg() != other.reg()) { \
        info_msg(msg.arg(#reg, toHexStr(reg()), toHexStr(other.reg()))); \
        equal = false; \
    }

namespace STM32 {
namespace WB55 {

OptionBytes::OptionBytes():
    m_isValid(false)
{}

OptionBytes::OptionBytes(const QByteArray &data):
    m_isValid(data.size() == OPTION_BYTES_SIZE),
    m_data(*(OptionBytesData*)(data.data()))
{}

OptionBytes::OptionBytes(QIODevice *file)
{
    while(!file->atEnd()) {
        const auto tokens = file->readLine().split(':');
        check_return_void(tokens.size() == 3, "Malformed Option Bytes file");

        QRegExp alphanum("[^A-Za-z0-9_]");
        QRegExp hex("[^0-9xXA-Fa-f]");

        check_return_void(alphanum.indexIn(tokens[0]) < 0, QString("Illegal character(s) in the register name: ") + tokens[0]);
        check_return_void(hex.indexIn(tokens[1]) < 0, QString("Illegal character(s) in the register value: ") + tokens[1]);

        bool canConvert = false;
        const auto value = tokens[1].toUInt(&canConvert, 16);
        check_return_void(canConvert, QString("Cannot convert given register value to a number: ") + tokens[1]);

        if(tokens[0] == "RDP") {
            m_data.word1[NORMAL].RDP = value;
            m_data.word1[COMPLEMENT].RDP = ~value;

        } else if(tokens[0] == "ESE") {
            m_data.word1[NORMAL].ESE = value;
            m_data.word1[COMPLEMENT].ESE = !value;

        } else if(tokens[0] == "BOR_LEV") {
            m_data.word1[NORMAL].BOR_LEV = value;
            m_data.word1[COMPLEMENT].BOR_LEV = ~value;

        } else if(tokens[0] == "nBOOT0") {
            m_data.word1[NORMAL].nBOOT0 = value;
            m_data.word1[COMPLEMENT].nBOOT0 = !value;

        } else if(tokens[0] == "nBOOT1") {
            m_data.word1[NORMAL].nBOOT1 = value;
            m_data.word1[COMPLEMENT].nBOOT1 = !value;

        } else if(tokens[0] == "nSWBOOT0") {
            m_data.word1[NORMAL].nSWBOOT0 = value;
            m_data.word1[COMPLEMENT].nSWBOOT0  = !value;

        } else if(tokens[0] == "SRAM2RST") {
            m_data.word1[NORMAL].SRAM2RST = value;
            m_data.word1[COMPLEMENT].SRAM2RST  = !value;

        } else if(tokens[0] == "SRAM2PE") {
            m_data.word1[NORMAL].SRAM2PE = value;
            m_data.word1[COMPLEMENT].SRAM2PE  = !value;

        } else if(tokens[0] == "nRST_STOP") {
            m_data.word1[NORMAL].nRST_STOP = value;
            m_data.word1[COMPLEMENT].nRST_STOP  = !value;

        } else if(tokens[0] == "nRST_STDBY") {
            m_data.word1[NORMAL].nRST_STDBY = value;
            m_data.word1[COMPLEMENT].nRST_STDBY  = !value;

        } else if(tokens[0] == "nRSTSHDW") {
            m_data.word1[NORMAL].nRSTSHDW = value;
            m_data.word1[COMPLEMENT].nRSTSHDW   = !value;

        } else if(tokens[0] == "WWDGSW") {
            m_data.word1[NORMAL].WWDGSW = value;
            m_data.word1[COMPLEMENT].WWDGSW = !value;

        } else if((tokens[0] == "IWDGSTDBY") || (tokens[0] == "IWGDSTDBY")) { // Temporary workaround for a typo
            m_data.word1[NORMAL].IWDGSTDBY = value;
            m_data.word1[COMPLEMENT].IWDGSTDBY = !value;

        } else if(tokens[0] == "IWDGSTOP") {
            m_data.word1[NORMAL].IWDGSTOP = value;
            m_data.word1[COMPLEMENT].IWDGSTOP = !value;

        } else if(tokens[0] == "IWDGSW") {
            m_data.word1[NORMAL].IWDGSW = value;
            m_data.word1[COMPLEMENT].IWDGSW = !value;

        } else if(tokens[0] == "AGCTRIM") {
            m_data.word1[NORMAL].AGCTRIM = value;
            m_data.word1[COMPLEMENT].AGCTRIM = ~value;

        } else if(tokens[0] == "PCROP1A_STRT") {
            m_data.word2[NORMAL].PCROP1A_STRT = value;
            m_data.word2[COMPLEMENT].PCROP1A_STRT = ~value;

        } else if(tokens[0] == "PCROP1A_END") {
            m_data.word3[NORMAL].PCROP1A_END = value;
            m_data.word3[COMPLEMENT].PCROP1A_END = ~value;

        } else if(tokens[0] == "PCROP_RDP") {
            m_data.word3[NORMAL].PCROP_RDP = value;
            m_data.word3[COMPLEMENT].PCROP_RDP = !value;

        } else if(tokens[0] == "WRP1A_STRT") {
            m_data.word4[NORMAL].WRP1A_STRT = value;
            m_data.word4[COMPLEMENT].WRP1A_STRT = ~value;

        } else if(tokens[0] == "WRP1A_END") {
            m_data.word4[NORMAL].WRP1A_END = value;
            m_data.word4[COMPLEMENT].WRP1A_END = ~value;

        } else if(tokens[0] == "WRP1B_STRT") {
            m_data.word5[NORMAL].WRP1B_STRT = value;
            m_data.word5[COMPLEMENT].WRP1B_STRT = ~value;

        } else if(tokens[0] == "WRP1B_END") {
            m_data.word5[NORMAL].WRP1B_END = value;
            m_data.word5[COMPLEMENT].WRP1B_END = ~value;

        } else if(tokens[0] == "PCROP1B_STRT") {
            m_data.word6[NORMAL].PCROP1B_STRT = value;
            m_data.word6[COMPLEMENT].PCROP1B_STRT = ~value;

        } else if(tokens[0] == "PCROP1B_END") {
            m_data.word7[NORMAL].PCROP1B_END = value;
            m_data.word7[COMPLEMENT].PCROP1B_END = ~value;

        } else if(tokens[0] == "IPCCDBA") {
            m_data.word8[NORMAL].IPCCDBA = value;
            m_data.word8[COMPLEMENT].IPCCDBA = ~value;

        } else if(tokens[0] == "SFSA") {
            m_data.word9[NORMAL].SFSA = value;
            m_data.word9[COMPLEMENT].SFSA = ~value;

        } else if(tokens[0] == "FSD") {
            m_data.word9[NORMAL].FSD = value;
            m_data.word9[COMPLEMENT].FSD = !value;

        } else if(tokens[0] == "DDS") {
            m_data.word9[NORMAL].DDS = value;
            m_data.word9[COMPLEMENT].DDS = !value;

        } else if(tokens[0] == "C2OPT") {
            m_data.word10[NORMAL].C2OPT = value;
            m_data.word10[COMPLEMENT].C2OPT = !value;

        } else if(tokens[0] == "NBRSD") {
            m_data.word10[NORMAL].NBRSD = value;
            m_data.word10[COMPLEMENT].NBRSD = !value;

        } else if(tokens[0] == "SNBRSA") {
            m_data.word10[NORMAL].SNBRSA = value;
            m_data.word10[COMPLEMENT].SNBRSA = ~value;

        } else if(tokens[0] == "BRSD") {
            m_data.word10[NORMAL].BRSD = value;
            m_data.word10[COMPLEMENT].BRSD = !value;

        } else if(tokens[0] == "SBRSA") {
            m_data.word10[NORMAL].SBRSA = value;
            m_data.word10[COMPLEMENT].SBRSA = ~value;

        } else if(tokens[0] == "SBRV") {
            m_data.word10[NORMAL].SBRV = value;
            m_data.word10[COMPLEMENT].SBRV = ~value;

        } else {
            error_msg(QString("Unexpected register name: ") + tokens[0]);
            return;
        }
    }

    m_isValid = true;
}

bool OptionBytes::operator ==(const OptionBytes &other) const
{
    bool equal = true;
    const QString msg = "Option Bytes mismatch @%1: this: 0x%2, other: 0x%3";

    check_register(equal, RDP, msg);
    check_register(equal, ESE, msg);
    check_register(equal, BOR_LEV, msg);
    check_register(equal, nRST_STOP, msg);
    check_register(equal, nRST_STDBY, msg);
    check_register(equal, nRSTSHDW, msg);
    check_register(equal, IWDGSW, msg);
    check_register(equal, IWDGSTOP, msg);
    check_register(equal, IWDGSTDBY, msg);
    check_register(equal, WWDGSW, msg);
    check_register(equal, nBOOT0, msg);
    check_register(equal, nBOOT1, msg);
    check_register(equal, nSWBOOT0, msg);
    check_register(equal, SRAM2PE, msg);
    check_register(equal, SRAM2RST, msg);
    check_register(equal, PCROP1A_STRT, msg);
    check_register(equal, PCROP1A_END, msg);
    check_register(equal, PCROP1B_STRT, msg);
    check_register(equal, PCROP1B_END, msg);
    check_register(equal, PCROP_RDP, msg);
    check_register(equal, WRP1A_STRT, msg);
    check_register(equal, WRP1A_END, msg);
    check_register(equal, WRP1B_STRT, msg);
    check_register(equal, WRP1B_END, msg);
    check_register(equal, IPCCDBA, msg);
    check_register(equal, SFSA, msg);
    check_register(equal, FSD, msg);
    check_register(equal, DDS, msg);
    check_register(equal, SBRV, msg);
    check_register(equal, SBRSA, msg);
    check_register(equal, BRSD, msg);
    check_register(equal, SNBRSA, msg);
    check_register(equal, NBRSD, msg);
    check_register(equal, C2OPT, msg);

    return equal;
}

bool OptionBytes::operator !=(const OptionBytes &other) const
{
    return !(*this == other);
}

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

bool OptionBytes::nBOOT0() const
{
    return m_data.word1[NORMAL].nBOOT0;
}

bool OptionBytes::nBOOT1() const
{
    return m_data.word1[NORMAL].nBOOT1;
}

bool OptionBytes::nSWBOOT0() const
{
    return m_data.word1[NORMAL].nSWBOOT0;
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

bool OptionBytes::PCROP_RDP() const
{
    return m_data.word3[NORMAL].PCROP_RDP;
}

uint8_t OptionBytes::RDP() const
{
    return m_data.word1[NORMAL].RDP;
}

bool OptionBytes::ESE() const
{
    return m_data.word1[NORMAL].ESE;
}

uint8_t OptionBytes::BOR_LEV() const
{
    return m_data.word1[NORMAL].BOR_LEV;
}

bool OptionBytes::nRST_STOP() const
{
    return m_data.word1[NORMAL].nRST_STOP;
}

bool OptionBytes::nRST_STDBY() const
{
    return m_data.word1[NORMAL].nRST_STDBY;
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

bool OptionBytes::IWDGSTDBY() const
{
    return m_data.word1[NORMAL].IWDGSTDBY;
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

uint16_t OptionBytes::PCROP1A_STRT() const
{
    return m_data.word2[NORMAL].PCROP1A_STRT;
}

uint16_t OptionBytes::PCROP1A_END() const
{
    return m_data.word3[NORMAL].PCROP1A_END;
}

uint16_t OptionBytes::PCROP1B_STRT() const
{
    return m_data.word6[NORMAL].PCROP1B_STRT;
}

uint16_t OptionBytes::PCROP1B_END() const
{
    return m_data.word7[NORMAL].PCROP1B_END;
}

uint8_t OptionBytes::WRP1A_STRT() const
{
    return m_data.word4[NORMAL].WRP1A_STRT;
}

uint8_t OptionBytes::WRP1A_END() const
{
    return m_data.word4[NORMAL].WRP1A_END;
}

uint8_t OptionBytes::WRP1B_STRT() const
{
    return m_data.word5[NORMAL].WRP1B_STRT;
}

uint8_t OptionBytes::WRP1B_END() const
{
    return m_data.word5[NORMAL].WRP1B_END;
}

uint16_t OptionBytes::IPCCDBA() const
{
    return m_data.word8[NORMAL].IPCCDBA;
}

uint32_t OptionBytes::SBRV() const
{
    return m_data.word10[NORMAL].SBRV;
}

void OptionBytes::setNBOOT0(bool set)
{
    m_data.word1[NORMAL].nBOOT0 = set;
    m_data.word1[COMPLEMENT].nBOOT0 = !set;
}

void OptionBytes::setNBOOT1(bool set)
{
    m_data.word1[NORMAL].nBOOT1 = set;
    m_data.word1[COMPLEMENT].nBOOT1 = !set;
}

void OptionBytes::setNSWBOOT0(bool set)
{
    m_data.word1[NORMAL].nSWBOOT0 = set;
    m_data.word1[COMPLEMENT].nSWBOOT0 = !set;
}

void OptionBytes::setBOR_LEV(uint8_t val)
{
    m_data.word1[NORMAL].nSWBOOT0 = val;
    m_data.word1[COMPLEMENT].nSWBOOT0 = ~val;
}

void OptionBytes::setnRST_STOP(bool set)
{
    m_data.word1[NORMAL].nRST_STOP = set;
    m_data.word1[COMPLEMENT].nRST_STOP = !set;
}

void OptionBytes::setnRST_STDBY(bool set)
{
    m_data.word1[NORMAL].nRST_STDBY = set;
    m_data.word1[COMPLEMENT].nRST_STDBY = !set;
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
    m_data.word1[NORMAL].IWDGSTDBY = set;
    m_data.word1[COMPLEMENT].IWDGSTDBY = !set;
}

void OptionBytes::setWWDGSW(bool set)
{
    m_data.word1[NORMAL].WWDGSW = set;
    m_data.word1[COMPLEMENT].WWDGSW = !set;
}

}
}

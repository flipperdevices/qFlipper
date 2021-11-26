#include "optionbytes.h"

#include <QRegExp>
#include <QIODevice>

#include "debug.h"

#define OPTION_BYTES_SIZE 128

#define NORMAL 0
#define COMPLEMENT 1

namespace STM32 {
namespace WB55 {

OptionBytes::OptionBytes():
    m_isValid(false)
{}

OptionBytes::OptionBytes(const QByteArray &data):
    OptionBytes()
{
    setData(data);
}

OptionBytes::OptionBytes(QIODevice *file):
    OptionBytes()
{
    while(!file->atEnd()) {
        const auto tokens = file->readLine().split(':');
        check_return_void(tokens.size() == 3, "Malformed Option Bytes file");

        QRegExp alphanum("[^A-Za-z0-9_]");
        QRegExp hex("[^0-9xXA-Fa-f]");

        const auto &fieldName = tokens[0];
        const auto &hexValue = tokens[1];

        check_return_void(alphanum.indexIn(fieldName) < 0, QString("Illegal character(s) in the field name: ") + fieldName);
        check_return_void(hex.indexIn(hexValue) < 0, QString("Illegal character(s) in the field value: ") + hexValue);

        check_return_void(fieldNames().contains(fieldName), QString("Illegal field name: ") + fieldName);

        bool canConvert;
        const auto value = hexValue.toUInt(&canConvert, 16);
        check_return_void(canConvert, QString("Cannot convert given register value to a number: ") + hexValue);

        m_data.insert(fieldName, value);
    }

    m_isValid = true;
}

OptionBytes OptionBytes::invalid()
{
    return OptionBytes();
}

qint64 OptionBytes::size()
{
    return OPTION_BYTES_SIZE;
}

const QVector<QByteArray> &OptionBytes::fieldNames()
{
    const static QVector<QByteArray> names {
        "RDP",
        "BOR_LEV",
        "nBOOT0",
        "nBOOT1",
        "nSWBOOT0",
        "SRAM2RST",
        "SRAM2PE",
        "nRST_STOP",
        "nRST_STDBY",
        "nRSTSHDW",
        "WWDGSW",
        "IWDGSTDBY",
        "IWGDSTDBY", // correcting for STM's derp
        "IWDGSTOP",
        "IWDGSW",
        "IPCCDBA",

        "ESE",
        "SFSA",
        "FSD",
        "DDS",
        "C2OPT",
        "NBRSD",
        "SNBRSA",
        "BRSD",
        "SBRSA",
        "SBRV",

        "PCROP1A_STRT",
        "PCROP1A_END",
        "PCROP_RDP",
        "PCROP1B_STRT",
        "PCROP1B_END",

        "WRP1A_STRT",
        "WRP1A_END",
        "WRP1B_STRT",
        "WRP1B_END",

        "AGC_TRIM"
    };

    return names;
}

bool OptionBytes::isValid() const
{
    return m_isValid;
}

QByteArray OptionBytes::data() const
{
    OptionBytesData ds;

    for(auto it = m_data.constKeyValueBegin(); it != m_data.constKeyValueEnd(); ++it) {
        const auto &fieldName = (*it).first;
        const auto value = (*it).second;

        if(fieldName == fieldNames()[0]) {
            ds.word1[NORMAL].RDP = value;
            ds.word1[COMPLEMENT].RDP = ~value;
        } else if(fieldName == fieldNames()[1]) {
            ds.word1[NORMAL].BOR_LEV = value;
            ds.word1[COMPLEMENT].BOR_LEV = ~value;
        } else if(fieldName == fieldNames()[2]) {
            ds.word1[NORMAL].nBOOT0 = value;
            ds.word1[COMPLEMENT].nBOOT0 = !value;
        } else if(fieldName == fieldNames()[3]) {
            ds.word1[NORMAL].nBOOT1 = value;
            ds.word1[COMPLEMENT].nBOOT1 = !value;
        } else if(fieldName == fieldNames()[4]) {
            ds.word1[NORMAL].nSWBOOT0 = value;
            ds.word1[COMPLEMENT].nSWBOOT0 = !value;
        } else if(fieldName == fieldNames()[5]) {
            ds.word1[NORMAL].SRAM2RST = value;
            ds.word1[COMPLEMENT].SRAM2RST = !value;
        } else if(fieldName == fieldNames()[6]) {
            ds.word1[NORMAL].SRAM2PE = value;
            ds.word1[COMPLEMENT].SRAM2PE = !value;
        } else if(fieldName == fieldNames()[7]) {
            ds.word1[NORMAL].nRST_STOP = value;
            ds.word1[COMPLEMENT].nRST_STOP = !value;
        } else if(fieldName == fieldNames()[8]) {
            ds.word1[NORMAL].nRST_STDBY = value;
            ds.word1[COMPLEMENT].nRST_STDBY = !value;
        } else if(fieldName == fieldNames()[9]) {
            ds.word1[NORMAL].nRSTSHDW = value;
            ds.word1[COMPLEMENT].nRSTSHDW = !value;
        } else if(fieldName == fieldNames()[10]) {
            ds.word1[NORMAL].WWDGSW = value;
            ds.word1[COMPLEMENT].WWDGSW = !value;
        } else if(fieldName == fieldNames()[11]) {
            ds.word1[NORMAL].IWDGSTDBY = value;
            ds.word1[COMPLEMENT].IWDGSTDBY = !value;
        } else if(fieldName == fieldNames()[13]) { // no index 12 is intentional
            ds.word1[NORMAL].IWDGSTOP = value;
            ds.word1[COMPLEMENT].IWDGSTOP = !value;
        } else if(fieldName == fieldNames()[14]) {
            ds.word1[NORMAL].IWDGSW = value;
            ds.word1[COMPLEMENT].IWDGSW = !value;
        } else if(fieldName == fieldNames()[15]) {
            ds.word8[NORMAL].IPCCDBA = value;
            ds.word8[COMPLEMENT].IPCCDBA = ~value;

        } else if(fieldName == fieldNames()[16]) {
            ds.word1[NORMAL].ESE = value;
            ds.word1[COMPLEMENT].ESE = !value;
        } else if(fieldName == fieldNames()[17]) {
            ds.word9[NORMAL].SFSA = value;
            ds.word9[COMPLEMENT].SFSA = ~value;
        } else if(fieldName == fieldNames()[18]) {
            ds.word9[NORMAL].FSD = value;
            ds.word9[COMPLEMENT].FSD = !value;
        } else if(fieldName == fieldNames()[19]) {
            ds.word9[NORMAL].DDS = value;
            ds.word9[COMPLEMENT].DDS = !value;
        } else if(fieldName == fieldNames()[20]) {
            ds.word10[NORMAL].C2OPT = value;
            ds.word10[COMPLEMENT].C2OPT = !value;
        } else if(fieldName == fieldNames()[21]) {
            ds.word10[NORMAL].NBRSD = value;
            ds.word10[COMPLEMENT].NBRSD = !value;
        } else if(fieldName == fieldNames()[22]) {
            ds.word10[NORMAL].SNBRSA = value;
            ds.word10[COMPLEMENT].SNBRSA = ~value;
        } else if(fieldName == fieldNames()[23]) {
            ds.word10[NORMAL].BRSD = value;
            ds.word10[COMPLEMENT].BRSD = !value;
        } else if(fieldName == fieldNames()[24]) {
            ds.word10[NORMAL].SBRSA = value;
            ds.word10[COMPLEMENT].SBRSA = ~value;
        } else if(fieldName == fieldNames()[25]) {
            ds.word10[NORMAL].SBRV = value;
            ds.word10[COMPLEMENT].SBRV = ~value;

        } else if(fieldName == fieldNames()[26]) {
            ds.word2[NORMAL].PCROP1A_STRT = value;
            ds.word2[COMPLEMENT].PCROP1A_STRT = ~value;
        } else if(fieldName == fieldNames()[27]) {
            ds.word3[NORMAL].PCROP1A_END = value;
            ds.word3[COMPLEMENT].PCROP1A_END = ~value;
        } else if(fieldName == fieldNames()[28]) {
            ds.word3[NORMAL].PCROP_RDP = value;
            ds.word3[COMPLEMENT].PCROP_RDP = !value;
        } else if(fieldName == fieldNames()[29]) {
            ds.word6[NORMAL].PCROP1B_STRT = value;
            ds.word6[COMPLEMENT].PCROP1B_STRT = ~value;
        } else if(fieldName == fieldNames()[30]) {
            ds.word7[NORMAL].PCROP1B_END = value;
            ds.word7[COMPLEMENT].PCROP1B_END = ~value;

        } else if(fieldName == fieldNames()[31]) {
            ds.word4[NORMAL].WRP1A_STRT = value;
            ds.word4[COMPLEMENT].WRP1A_STRT  = ~value;
        } else if(fieldName == fieldNames()[32]) {
            ds.word4[NORMAL].WRP1A_END = value;
            ds.word4[COMPLEMENT].WRP1A_END  = ~value;
        } else if(fieldName == fieldNames()[33]) {
            ds.word5[NORMAL].WRP1B_STRT = value;
            ds.word5[COMPLEMENT].WRP1B_STRT  = ~value;
        } else if(fieldName == fieldNames()[34]) {
            ds.word5[NORMAL].WRP1B_END = value;
            ds.word5[COMPLEMENT].WRP1B_END  = ~value;
        } else if(fieldName == fieldNames()[35]) {
            ds.word1[NORMAL].AGC_TRIM = value;
            ds.word1[COMPLEMENT].AGC_TRIM  = ~value;
        } else {}
    }

    return QByteArray((const char*)(&ds), sizeof(OptionBytesData));
}

void OptionBytes::setData(const QByteArray &data)
{
    m_isValid = data.size() == OPTION_BYTES_SIZE;
    check_return_void(m_isValid, "Unexpected data size");

    m_data.clear();

    const auto ds = *((OptionBytesData*)data.data());

    m_data.insert(fieldNames()[ 0], ds.word1[NORMAL].RDP);
    m_data.insert(fieldNames()[ 1], ds.word1[NORMAL].BOR_LEV);
    m_data.insert(fieldNames()[ 2], ds.word1[NORMAL].nBOOT0);
    m_data.insert(fieldNames()[ 3], ds.word1[NORMAL].nBOOT1);
    m_data.insert(fieldNames()[ 4], ds.word1[NORMAL].nSWBOOT0);
    m_data.insert(fieldNames()[ 5], ds.word1[NORMAL].SRAM2RST);
    m_data.insert(fieldNames()[ 6], ds.word1[NORMAL].SRAM2PE);
    m_data.insert(fieldNames()[ 7], ds.word1[NORMAL].nRST_STOP);
    m_data.insert(fieldNames()[ 8], ds.word1[NORMAL].nRST_STDBY);
    m_data.insert(fieldNames()[ 9], ds.word1[NORMAL].nRSTSHDW);
    m_data.insert(fieldNames()[10], ds.word1[NORMAL].WWDGSW);
    m_data.insert(fieldNames()[11], ds.word1[NORMAL].IWDGSTDBY);
    m_data.insert(fieldNames()[12], ds.word1[NORMAL].IWDGSTDBY); // correcting for STM's derp
    m_data.insert(fieldNames()[13], ds.word1[NORMAL].IWDGSTOP);
    m_data.insert(fieldNames()[14], ds.word1[NORMAL].IWDGSW);
    m_data.insert(fieldNames()[15], ds.word8[NORMAL].IPCCDBA);

    m_data.insert(fieldNames()[16], ds.word1[NORMAL].ESE);
    m_data.insert(fieldNames()[17], ds.word9[NORMAL].SFSA);
    m_data.insert(fieldNames()[18], ds.word9[NORMAL].FSD);
    m_data.insert(fieldNames()[19], ds.word9[NORMAL].DDS);
    m_data.insert(fieldNames()[20], ds.word10[NORMAL].C2OPT);
    m_data.insert(fieldNames()[21], ds.word10[NORMAL].NBRSD);
    m_data.insert(fieldNames()[22], ds.word10[NORMAL].SNBRSA);
    m_data.insert(fieldNames()[23], ds.word10[NORMAL].BRSD);
    m_data.insert(fieldNames()[24], ds.word10[NORMAL].SBRSA);
    m_data.insert(fieldNames()[25], ds.word10[NORMAL].SBRV);

    m_data.insert(fieldNames()[26], ds.word2[NORMAL].PCROP1A_STRT);
    m_data.insert(fieldNames()[27], ds.word3[NORMAL].PCROP1A_END);
    m_data.insert(fieldNames()[28], ds.word3[NORMAL].PCROP_RDP);
    m_data.insert(fieldNames()[29], ds.word6[NORMAL].PCROP1B_STRT);
    m_data.insert(fieldNames()[30], ds.word7[NORMAL].PCROP1B_END);

    m_data.insert(fieldNames()[31], ds.word4[NORMAL].WRP1A_STRT);
    m_data.insert(fieldNames()[32], ds.word4[NORMAL].WRP1A_END);
    m_data.insert(fieldNames()[33], ds.word5[NORMAL].WRP1B_STRT);
    m_data.insert(fieldNames()[34], ds.word5[NORMAL].WRP1B_END);

    m_data.insert(fieldNames()[35], ds.word1[NORMAL].AGC_TRIM);
}

uint32_t OptionBytes::value(const QByteArray &fieldName) const
{
    check_return_val(fieldNames().contains(fieldName), QString("Illegal field name: ") + fieldName, std::numeric_limits<uint32_t>::max());
    check_return_val(m_data.contains(fieldName), QString("Field name allowed, but not set: ") + fieldName, std::numeric_limits<uint32_t>::max());

    return m_data[fieldName];
}

void OptionBytes::setValue(const QByteArray &fieldName, uint32_t value)
{
    check_return_void(fieldNames().contains(fieldName), QString("Illegal field name: ") + fieldName);
    m_data[fieldName] = value;
}

OptionBytes::DataMap OptionBytes::compare(const OptionBytes &other) const
{
    DataMap cmp;

    for(auto it = other.m_data.constKeyValueBegin(); it != other.m_data.constKeyValueEnd(); ++it) {
        const auto &fieldName = (*it).first;

        if(m_data.contains(fieldName)) {
            const auto right = (*it).second;
            const auto left = m_data[fieldName];

            if(left != right) {
                cmp.insert(fieldName, right);
            }

        } else {
            cmp.insert(fieldName, (*it).second);
        }
    }

    return cmp;
}

OptionBytes OptionBytes::corrected(const DataMap &diff) const
{
    OptionBytes res = (*this);

    for(auto it = diff.constKeyValueBegin(); it != diff.constKeyValueEnd(); ++it) {
        res.setValue((*it).first, (*it).second);
    }

    return res;
}

}
}

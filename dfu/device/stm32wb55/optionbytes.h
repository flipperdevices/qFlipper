#pragma once

#include <QHash>
#include <QString>
#include <QVector>
#include <QByteArray>

class QIODevice;

namespace STM32 {
namespace WB55 {

class OptionBytes
{

#pragma pack(push,1)
    struct OptionBytesData {
        struct Word1 {
            unsigned RDP:8;
            unsigned ESE:1;
            unsigned BOR_LEV:3;
            unsigned nRST_STOP:1;
            unsigned nRST_STDBY:1;
            unsigned nRSTSHDW:1;
            unsigned UNUSED1:1;
            unsigned IWDGSW:1;
            unsigned IWDGSTOP:1;
            unsigned IWDGSTDBY:1;
            unsigned WWDGSW:1;
            unsigned UNUSED2:3;
            unsigned nBOOT1:1;
            unsigned SRAM2PE:1;
            unsigned SRAM2RST:1;
            unsigned nSWBOOT0:1;
            unsigned nBOOT0:1;
            unsigned UNUSED3:1;
            unsigned AGC_TRIM:3;
        };

        struct Word2 {
            unsigned PCROP1A_STRT:9;
            unsigned UNUSED:23;
        };

        struct Word3 {
            unsigned PCROP1A_END:9;
            unsigned UNUSED:22;
            unsigned PCROP_RDP:1;
        };

        struct Word4 {
            uint8_t WRP1A_STRT;
            uint8_t UNUSED1;
            uint8_t WRP1A_END;
            uint8_t UNUSED2;
        };

        struct Word5 {
            uint8_t WRP1B_STRT;
            uint8_t UNUSED1;
            uint8_t WRP1B_END;
            uint8_t UNUSED2;
        };

        struct Word6 {
            unsigned PCROP1B_STRT:9;
            unsigned UNUSED:23;
        };

        struct Word7 {
            unsigned PCROP1B_END:9;
            unsigned UNUSED:23;
        };

        struct Word8 {
            unsigned IPCCDBA:14;
            unsigned UNUSED:18;
        };

        struct Word9 {
            unsigned SFSA:8;
            unsigned FSD:1;
            unsigned UNUSED1:3;
            unsigned DDS:1;
            unsigned UNUSED2:19;
        };

        struct Word10 {
            unsigned SBRV:18;
            unsigned SBRSA:5;
            unsigned BRSD:1;
            unsigned UNUSED1:1;
            unsigned SNBRSA:5;
            unsigned NBRSD:1;
            unsigned C2OPT:1;
        };

        Word1 word1[2];
        Word2 word2[2];
        Word3 word3[2];
        Word4 word4[2];
        Word5 word5[2];
        Word6 word6[2];
        Word7 word7[2];

        uint32_t UNUSED[12];

        Word8 word8[2];
        Word9 word9[2];
        Word10 word10[2];
    };

    static_assert (sizeof(OptionBytesData) == 128, "Check your struct packing!");

#pragma pack(pop)

    OptionBytes();

public:
    using DataMap = QHash<QByteArray, uint32_t>;

    OptionBytes(const QByteArray &data);
    OptionBytes(QIODevice *file);

    static OptionBytes invalid();
    static qint64 size();

    static const QVector<QByteArray> &fieldNames();

    bool isValid() const;

    QByteArray data() const;
    void setData(const QByteArray &data);

    uint32_t value(const QByteArray &fieldName) const;
    void setValue(const QByteArray &fieldName, uint32_t value);

    DataMap compare(const OptionBytes &other) const;
    OptionBytes corrected(const DataMap &diff) const;

private:
    DataMap m_data;
    bool m_isValid;
};

}
}

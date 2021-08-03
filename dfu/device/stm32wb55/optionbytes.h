#pragma once

#include <QString>
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
            unsigned AGCTRIM:3;
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
    OptionBytes(const QByteArray &data);
    OptionBytes(QIODevice *file);

    bool operator ==(const OptionBytes &other) const;
    bool operator !=(const OptionBytes &other) const;

    static OptionBytes invalid();
    static qint64 size();

    bool isValid() const;
    QByteArray data() const;

    uint8_t RDP() const;

    bool ESE() const;

    uint8_t BOR_LEV() const;

    bool nRST_STOP() const;
    bool nRST_STDBY() const;
    bool nRSTSHDW() const;

    bool IWDGSW() const;
    bool IWDGSTOP() const;
    bool IWDGSTDBY() const;
    bool WWDGSW() const;

    bool nBOOT0() const;
    bool nBOOT1() const;
    bool nSWBOOT0() const;

    bool SRAM2PE() const;
    bool SRAM2RST() const;

    uint8_t AGCTRIM() const;

    bool FSD() const;
    bool DDS() const;
    bool BRSD() const;
    bool NBRSD() const;
    bool C2OPT() const;
    bool PCROP_RDP() const;

    uint8_t SFSA() const;

    uint8_t SBRSA() const;
    uint8_t SNBRSA() const;

    uint16_t PCROP1A_STRT() const;
    uint16_t PCROP1A_END() const;

    uint16_t PCROP1B_STRT() const;
    uint16_t PCROP1B_END() const;

    uint8_t WRP1A_STRT() const;
    uint8_t WRP1A_END() const;
    uint8_t WRP1B_STRT() const;
    uint8_t WRP1B_END() const;

    uint16_t IPCCDBA() const;
    uint32_t SBRV() const;

    void setNBOOT0(bool set);
    void setNBOOT1(bool set);
    void setNSWBOOT0(bool set);

    void setSRAM2RST(bool set);
    void setSRAM2PE(bool set);

    void setBOR_LEV(uint8_t val);

    void setnRST_STOP(bool set);
    void setnRST_STDBY(bool set);
    void setnRSTSHDW(bool set);
    void setIWDGSW(bool set);
    void setIWDGSTOP(bool set);
    void setIWGDSTDBY(bool set);
    void setWWDGSW(bool set);

private:
    bool m_isValid;
    OptionBytesData m_data;
};

}
}

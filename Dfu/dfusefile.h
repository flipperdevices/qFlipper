#ifndef DFUSEFILE_H
#define DFUSEFILE_H

#include <QList>
#include <QIODevice>
#include <QByteArray>

class DfuseFile
{
public:
    struct Prefix {
        QByteArray szSignature;
        uint32_t DFUImageSize;
        uint8_t bVersion;
        uint8_t bTargets;
    };

    struct Suffix {
        uint16_t bcdDevice;
        uint16_t idProduct;
        uint16_t idVendor;
        uint16_t bcdDFU;
        uint32_t ucDFUSignature;
        uint8_t bLength;
        uint32_t dwCRC;
    };

    struct Image {
        struct TargetPrefix {
            QByteArray szSignature;
            uint8_t bAlternateSetting;
            bool bTargetNamed;
            QByteArray szTargetName;
            uint32_t dwtargetSize;
            uint32_t dwNbElements;
        };

        struct Element {
            uint32_t dwElementAddress;
            uint32_t dwElementSize;
            QByteArray data;
        };

        TargetPrefix prefix;
        QList<Element> elements;
    };

    DfuseFile(QIODevice *file);

    bool isValid() const;

    const Prefix &prefix() const;
    const Suffix &suffix() const;
    QList<Image> &images();

private:
    bool readPrefix(QIODevice *file);
    bool readSuffix(QIODevice *file);
    bool readImage(QIODevice *file);
    void readElement(QIODevice *file);
    bool checkCRC(QIODevice *file);

    static uint32_t generateCRC(QIODevice *file);

    bool m_isValid;
    Prefix m_prefix;
    Suffix m_suffix;

    QList<Image> m_images;
};

#endif // DFUSEFILE_H

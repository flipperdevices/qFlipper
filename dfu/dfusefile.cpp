#include "dfusefile.h"

#include <QDebug>

#include "debug.h"

static const auto DFUSE_SUFFIX_SIZE = 16;

DfuseFile::DfuseFile(QIODevice *file)
{
    m_isValid = readPrefix(file);
    check_return_void(m_isValid, "Failed to read DfuSe file prefix");

    for(auto i = 0; i < m_prefix.bTargets; ++i) {
        m_isValid = readImage(file);
        check_return_void(m_isValid, "Failed to read DfuSe image");
    }

    m_isValid = readSuffix(file);
    check_return_void(m_isValid, "No valid DfuSe suffix");

    file->reset();

    m_isValid = checkCRC(file);
    check_return_void(m_isValid, "Checksum doesn't match");
}

bool DfuseFile::isValid() const
{
    return m_isValid;
}

const DfuseFile::Prefix &DfuseFile::prefix() const
{
    return m_prefix;
}

const DfuseFile::Suffix &DfuseFile::suffix() const
{
    return m_suffix;
}

QList<DfuseFile::Image> &DfuseFile::images()
{
    return m_images;
}

bool DfuseFile::readPrefix(QIODevice *file)
{
    const auto DFUSE_PREFIX_SIZE = 11;
    const auto PREFIX_SIGNATURE_SIZE = 5;

    check_return_bool(file->bytesAvailable() >= DFUSE_PREFIX_SIZE, "File is too short");

    m_prefix.szSignature = file->read(PREFIX_SIGNATURE_SIZE);
    check_return_bool(m_prefix.szSignature == "DfuSe", "Not a valid DfuSe file");

    file->read((char*)&m_prefix.bVersion, sizeof(uint8_t));
    file->read((char*)&m_prefix.DFUImageSize, sizeof(uint32_t));
    file->read((char*)&m_prefix.bTargets, sizeof(uint8_t));

    check_return_bool(m_prefix.DFUImageSize == (file->size() - DFUSE_SUFFIX_SIZE), "File size does not match");

    return true;
}

bool DfuseFile::readSuffix(QIODevice *file)
{
    m_suffix.ucDFUSignature = 0; // Just to be safe - we're reading only 3 bytes into it

    file->read((char*)&m_suffix.bcdDevice, sizeof(uint16_t));
    file->read((char*)&m_suffix.idProduct, sizeof(uint16_t));
    file->read((char*)&m_suffix.idVendor, sizeof(uint16_t));
    file->read((char*)&m_suffix.bcdDFU, sizeof(uint16_t));
    file->read((char*)&m_suffix.ucDFUSignature, 3);
    file->read((char*)&m_suffix.bLength, sizeof(uint8_t));
    file->read((char*)&m_suffix.dwCRC, sizeof(uint32_t));

    const auto res = (m_suffix.ucDFUSignature == 0x444655) && (m_suffix.bLength == DFUSE_SUFFIX_SIZE);
    check_continue(res, "Failed to read DfuSe suffix");

    return res;
}

bool DfuseFile::readImage(QIODevice *file)
{
    const auto TARGET_SIGNATURE_SIZE = 6;
    const auto TARGET_NAME_SIZE = 255;

    Image img;

    img.prefix.szSignature = file->read(TARGET_SIGNATURE_SIZE);
    check_return_bool(img.prefix.szSignature == "Target", "Not a valid DfuSe target image");

    file->read((char*)&img.prefix.bAlternateSetting, sizeof(uint8_t));
    file->read((char*)&img.prefix.bTargetNamed, sizeof(uint32_t));

    img.prefix.szTargetName = file->read(TARGET_NAME_SIZE);

    file->read((char*)&img.prefix.dwtargetSize, sizeof(uint32_t));
    file->read((char*)&img.prefix.dwNbElements, sizeof(uint32_t));

    m_images.append(img);

    for(uint32_t i = 0; i < img.prefix.dwNbElements; ++i) {
        readElement(file);
    }

    return true;
}

void DfuseFile::readElement(QIODevice *file)
{
    Image::Element el;

    file->read((char*)&el.dwElementAddress, sizeof(uint32_t));
    file->read((char*)&el.dwElementSize, sizeof(uint32_t));
    el.data = file->read(el.dwElementSize);

    m_images.last().elements.append(el);
}

bool DfuseFile::checkCRC(QIODevice *file)
{
    return m_suffix.dwCRC == generateCRC(file);
}

uint32_t DfuseFile::generateCRC(QIODevice *file)
{
    const auto LUT_SIZE = 256;
    uint32_t lut[LUT_SIZE];

    for(auto i = 0; i < LUT_SIZE; ++i) {
        uint32_t val = i;

        for(auto j = 0; j < 8; ++j) {
            val = (val & 1U) ? 0xEDB88320U ^ (val >> 1) : val >> 1;
        }

        lut[i] = val;
    }

    uint32_t val = UINT32_MAX;
    uint8_t c;

    while((size_t)file->bytesAvailable() > sizeof(uint32_t)) {
        file->getChar((char*)&c);
        val = lut[(val ^ c) & UINT8_MAX] ^ (val >> 8);
    }

    return val;
}

#include "regiondata.h"

#include <QDebug>

#include "pb_encode.h"

static bool fieldEncodeCallback(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    const BandInfoList *bands = (BandInfoList*)*arg;

    for(const auto &band : *bands) {
        if(!pb_encode_tag_for_field(stream, field)) {
            return false;
        }

        PB_Region_Band bandData = {
            band.start,
            band.end,
            (int8_t)band.powerLimit,
            (uint8_t)band.dutyCycle,
        };

        if(!pb_encode_submessage(stream, &PB_Region_Band_msg, &bandData)) {
            return false;
        }
    }

    return true;
}

RegionData::RegionData(const QByteArray &countryCode, const BandInfoList &bands):
    m_bands(bands)
{
    if(!countryCode.isEmpty()) {
        m_message.country_code = (pb_bytes_array_t*)malloc(PB_BYTES_ARRAY_T_ALLOCSIZE(countryCode.size()));
        m_message.country_code->size = countryCode.size();
        memcpy(m_message.country_code->bytes, countryCode.data(), countryCode.size());

    } else {
        m_message.country_code = nullptr;
    }

    m_message.bands.funcs.encode = fieldEncodeCallback;
    m_message.bands.arg = &m_bands;
}

RegionData::~RegionData()
{
    if(m_message.country_code) {
        free(m_message.country_code);
    }
}

const QByteArray RegionData::encode() const
{
    QByteArray ret;
    size_t encodedSize;

    if(!pb_get_encoded_size(&encodedSize, &PB_Region_msg, &m_message)) {
        return ret;
    }

    ret.resize((int)encodedSize);
    auto stream = pb_ostream_from_buffer((pb_byte_t*)ret.data(), ret.size());

    if(!pb_encode(&stream, &PB_Region_msg, &m_message)) {
       ret.clear();
    }

    return ret;
}

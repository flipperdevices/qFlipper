#include "protobufplugin.h"

#include "pb_encode.h"

#include "mainresponse.h"

ProtobufPlugin::ProtobufPlugin(QObject *parent):
    QObject(parent)
{}

const QByteArray ProtobufPlugin::testSystemPingResponse() const
{
    QByteArray ret;
    const char str[] = "Have you heard about Among Us, Gregory?";

    PB_Main msg = {
        66,
        PB_CommandStatus_ERROR_STORAGE_DIR_NOT_EMPTY,
        false,
        {{nullptr}, nullptr},
        PB_Main_system_ping_response_tag,
        {}
    };

    auto &response = msg.content.system_ping_response;

    response.data = (pb_bytes_array_t*)malloc(PB_BYTES_ARRAY_T_ALLOCSIZE(sizeof(str)));
    response.data->size = sizeof(str);

    memcpy(response.data->bytes, str, sizeof(str));

    do {
        pb_ostream_t s = PB_OSTREAM_SIZING;

        if(!pb_encode_ex(&s, &PB_Main_msg, &msg, PB_ENCODE_DELIMITED)) {
            break;
        }

        ret.resize(s.bytes_written);
        s = pb_ostream_from_buffer((pb_byte_t*)ret.data(), ret.size());

        if(!pb_encode_ex(&s, &PB_Main_msg, &msg, PB_ENCODE_DELIMITED)) {
           ret.clear();
        }

    } while(false);

    free(response.data);
    return ret;
}

const QByteArray ProtobufPlugin::systemPing(uint32_t id) const
{
    return QByteArray::number(id, 16);
}

const QByteArray ProtobufPlugin::systemDeviceInfo(uint32_t id) const
{
    return QByteArray::number(id, 16);
}

QObject *ProtobufPlugin::decode(const QByteArray &buffer, QObject *parent) const
{
    MessageWrapper wrp(buffer);
    return MainResponse::create(wrp, parent);
}

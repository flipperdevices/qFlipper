#include "protobufplugin.h"

#include "pb_encode.h"

#include "mainresponse.h"

#include "guirequest.h"
#include "statusrequest.h"
#include "systemrequest.h"
#include "storagerequest.h"

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

const QByteArray ProtobufPlugin::statusPing(uint32_t id, const QByteArray &data) const
{
    return StatusPingRequest(id, data).encode();
}

const QByteArray ProtobufPlugin::systemReboot(uint32_t id, RebootMode mode) const
{
    const auto rm = mode == RebootModeOS ? PB_System_RebootRequest_RebootMode_OS :
                                           PB_System_RebootRequest_RebootMode_DFU;

    return SystemRebootRequest(id, rm).encode();
}

const QByteArray ProtobufPlugin::systemFactoryReset(uint32_t id) const
{
    return SystemFactoryResetRequest(id).encode();
}

const QByteArray ProtobufPlugin::systemDeviceInfo(uint32_t id) const
{
    return SystemDeviceInfoRequest(id).encode();
}

const QByteArray ProtobufPlugin::systemGetDateTime(uint32_t id) const
{
    return SystemGetDateTimeRequest(id).encode();
}

const QByteArray ProtobufPlugin::systemSetDateTime(uint32_t id, const QDateTime &dateTime) const
{
    return SystemSetDateTimeRequest(id, dateTime).encode();
}

const QByteArray ProtobufPlugin::guiStartScreenStream(uint32_t id) const
{
    return GuiStartScreenStreamRequest(id).encode();
}

const QByteArray ProtobufPlugin::guiStopScreenStream(uint32_t id) const
{
    return GuiStopScreenStreamRequest(id).encode();
}

const QByteArray ProtobufPlugin::guiScreenFrame(uint32_t id, const QByteArray screenData) const
{
    return GuiScreenFrameRequest(id, screenData).encode();
}

const QByteArray ProtobufPlugin::guiSendInput(uint32_t id, int key, int type) const
{
    return GuiSendInputRequest(id, (PB_Gui_InputKey)key, (PB_Gui_InputType)type).encode();
}

const QByteArray ProtobufPlugin::guiStartVirtualDisplay(uint32_t id, const QByteArray screenData) const
{
    return GuiStartVirtualDisplayRequest(id, screenData).encode();
}

const QByteArray ProtobufPlugin::guiStopVirtualDisplay(uint32_t id) const
{
    return GuiStopVirtualDisplayRequest(id).encode();
}

const QByteArray ProtobufPlugin::storageInfo(uint32_t id, const QByteArray &path) const
{
    return StorageInfoRequest(id, path).encode();
}

const QByteArray ProtobufPlugin::storageStat(uint32_t id, const QByteArray &path) const
{
    return StorageStatRequest(id, path).encode();
}

const QByteArray ProtobufPlugin::storageList(uint32_t id, const QByteArray &path) const
{
    return StorageListRequest(id, path).encode();
}

const QByteArray ProtobufPlugin::storageMkDir(uint32_t id, const QByteArray &path) const
{
    return StorageMkDirRequest(id, path).encode();
}

const QByteArray ProtobufPlugin::storageRemove(uint32_t id, const QByteArray &path, bool recursive) const
{
    return StorageRemoveRequest(id, path, recursive).encode();
}

const QByteArray ProtobufPlugin::storageRead(uint32_t id, const QByteArray &path) const
{
    return StorageReadRequest(id, path).encode();
}

const QByteArray ProtobufPlugin::storageWrite(uint32_t id, const QByteArray &path, const QByteArray &data, bool hasNext) const
{
    return StorageWriteRequest(id, path, data, hasNext).encode();
}

QObject *ProtobufPlugin::decode(const QByteArray &buffer, QObject *parent) const
{
    MessageWrapper wrp(buffer);
    return MainResponse::create(wrp, parent);
}

#include "protobufplugin.h"

#include <QIODevice>

#include "mainresponse.h"

#include "guirequest.h"
#include "statusrequest.h"
#include "systemrequest.h"
#include "storagerequest.h"
#include "propertyrequest.h"

#include "regiondata.h"

ProtobufPlugin::ProtobufPlugin(QObject *parent):
    QObject(parent),
    m_versionMinor(0)
{}

uint32_t ProtobufPlugin::versionMajor() const
{
    return 0;
}

void ProtobufPlugin::setMinorVersion(uint32_t version)
{
    m_versionMinor = version;
}

const QByteArray ProtobufPlugin::statusPing(uint32_t id, const QByteArray &data) const
{
    return StatusPingRequest(id, data).encode();
}

const QByteArray ProtobufPlugin::systemReboot(uint32_t id, RebootMode mode) const
{
    PB_System_RebootRequest_RebootMode rm;

    if(mode == RebootModeOS) {
        rm = PB_System_RebootRequest_RebootMode_OS;
    } else if(mode == RebootModeRecovery) {
        rm = PB_System_RebootRequest_RebootMode_DFU;
    } else if(mode == RebootModeUpdate) {
        rm = PB_System_RebootRequest_RebootMode_UPDATE;
    } else {
        return QByteArray();
    }

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

const QByteArray ProtobufPlugin::systemUpdateRequest(uint32_t id, const QByteArray &manifestPath) const
{
    return SystemUpdateRequest(id, manifestPath).encode();
}

const QByteArray ProtobufPlugin::systemProtobufVersion(uint32_t id) const
{
    return SystemProtobufVersionRequest(id).encode();
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

const QByteArray ProtobufPlugin::storageRename(uint32_t id, const QByteArray &oldPath, const QByteArray &newPath) const
{
    return StorageRenameRequest(id, oldPath, newPath).encode();
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

const QByteArray ProtobufPlugin::storageMd5Sum(uint32_t id, const QByteArray &path) const
{
    return StorageMd5SumRequest(id, path).encode();
}

const QByteArray ProtobufPlugin::propertyGet(uint32_t id, const QByteArray &key) const
{
    return PropertyGetRequest(id, key).encode();
}

const QByteArray ProtobufPlugin::regionBands(const QByteArray &countryCode, const BandInfoList &bands) const
{
    return RegionData(countryCode, bands).encode();
}

QObject *ProtobufPlugin::decode(const QByteArray &buffer, QObject *parent) const
{
    MessageWrapper wrp(buffer);
    return MainResponse::create(wrp, parent);
}

#pragma once

#include <QtPlugin>
#include <QByteArray>

#include "bandinfo.h"

class ProtobufPluginInterface
{
public:
    enum RebootMode {
        RebootModeOS = 0,
        RebootModeRecovery = 1,
        RebootModeUpdate = 2,
    };

    virtual ~ProtobufPluginInterface() {}

    virtual uint32_t versionMajor() const = 0;
    virtual void setMinorVersion(uint32_t version) = 0;

    virtual const QByteArray statusPing(uint32_t id, const QByteArray &data = QByteArray()) const = 0;
    virtual const QByteArray systemFactoryReset(uint32_t id) const = 0;
    virtual const QByteArray systemReboot(uint32_t id, RebootMode mode) const = 0;
    virtual const QByteArray systemDeviceInfo(uint32_t id) const = 0;
    virtual const QByteArray systemGetDateTime(uint32_t id) const = 0;
    virtual const QByteArray systemSetDateTime(uint32_t id, const QDateTime &dateTime) const = 0;
    virtual const QByteArray systemUpdateRequest(uint32_t id, const QByteArray &manifestPath) const = 0;
    virtual const QByteArray systemProtobufVersion(uint32_t id) const = 0;

    virtual const QByteArray guiStartScreenStream(uint32_t id) const = 0;
    virtual const QByteArray guiStopScreenStream(uint32_t id) const = 0;
    virtual const QByteArray guiScreenFrame(uint32_t id, const QByteArray screenData = QByteArray()) const = 0;
    virtual const QByteArray guiSendInput(uint32_t id, int key, int type) const = 0;
    virtual const QByteArray guiStartVirtualDisplay(uint32_t id, const QByteArray screenData = QByteArray()) const = 0;
    virtual const QByteArray guiStopVirtualDisplay(uint32_t id) const = 0;

    virtual const QByteArray storageInfo(uint32_t id, const QByteArray &path) const = 0;
    virtual const QByteArray storageStat(uint32_t id, const QByteArray &path) const = 0;
    virtual const QByteArray storageList(uint32_t id, const QByteArray &path) const = 0;
    virtual const QByteArray storageMkDir(uint32_t id, const QByteArray &path) const = 0;
    virtual const QByteArray storageRename(uint32_t id, const QByteArray &oldPath, const QByteArray &newPath) const = 0;
    virtual const QByteArray storageRemove(uint32_t id, const QByteArray &path, bool recursive = false) const = 0;
    virtual const QByteArray storageRead(uint32_t id, const QByteArray &path) const = 0;
    virtual const QByteArray storageWrite(uint32_t id, const QByteArray &path, const QByteArray &data, bool hasNext) const = 0;
    virtual const QByteArray storageMd5Sum(uint32_t id, const QByteArray &path) const = 0;

    virtual const QByteArray propertyGet(uint32_t id, const QByteArray &key) const = 0;

    virtual const QByteArray regionBands(const QByteArray &countryCode, const BandInfoList &bands) const = 0;

    virtual QObject *decode(const QByteArray &buffer, QObject *parent = nullptr) const = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(ProtobufPluginInterface, "com.flipperdevices.ProtobufPluginInterface/1.0")
QT_END_NAMESPACE

#pragma once

#include <QObject>
#include <QtPlugin>

#include "protobufplugininterface.h"

class ProtobufPlugin : public QObject, public ProtobufPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.flipperdevices.ProtobufPluginInterface" FILE "protobufplugin.json")
    Q_INTERFACES(ProtobufPluginInterface)

public:
    ProtobufPlugin(QObject *parent = nullptr);

    uint32_t versionMajor() const override;
    void setMinorVersion(uint32_t version) override;

    const QByteArray statusPing(uint32_t id, const QByteArray &data) const override;

    const QByteArray systemReboot(uint32_t id, RebootMode mode) const override;
    const QByteArray systemFactoryReset(uint32_t id) const override;
    const QByteArray systemDeviceInfo(uint32_t id) const override;
    const QByteArray systemGetDateTime(uint32_t id) const override;
    const QByteArray systemSetDateTime(uint32_t id, const QDateTime &dateTime) const override;
    const QByteArray systemUpdateRequest(uint32_t id, const QByteArray &manifestPath) const override;
    const QByteArray systemProtobufVersion(uint32_t id) const override;

    const QByteArray guiStartScreenStream(uint32_t id) const override;
    const QByteArray guiStopScreenStream(uint32_t id) const override;
    const QByteArray guiScreenFrame(uint32_t id, const QByteArray screenData) const override;
    const QByteArray guiSendInput(uint32_t id, int key, int type) const override;
    const QByteArray guiStartVirtualDisplay(uint32_t id, const QByteArray screenData) const override;
    const QByteArray guiStopVirtualDisplay(uint32_t id) const override;

    const QByteArray storageInfo(uint32_t id, const QByteArray &path) const override;
    const QByteArray storageStat(uint32_t id, const QByteArray &path) const override;
    const QByteArray storageList(uint32_t id, const QByteArray &path) const override;
    const QByteArray storageMkDir(uint32_t id, const QByteArray &path) const override;
    const QByteArray storageRename(uint32_t id, const QByteArray &oldPath, const QByteArray &newPath) const override;
    const QByteArray storageRemove(uint32_t id, const QByteArray &path, bool recursive) const override;
    const QByteArray storageRead(uint32_t id, const QByteArray &path) const override;
    const QByteArray storageWrite(uint32_t id, const QByteArray &path, const QByteArray &data, bool hasNext) const override;
    const QByteArray storageMd5Sum(uint32_t id, const QByteArray &path) const override;

    const QByteArray propertyGet(uint32_t id, const QByteArray &key) const override;

    const QByteArray regionBands(const QByteArray &countryCode, const BandInfoList &bands) const override;

    QObject *decode(const QByteArray &buffer, QObject *parent = nullptr) const override;

private:
    uint32_t m_versionMinor;
};

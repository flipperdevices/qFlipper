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

    const QByteArray testSystemPingResponse() const override;

    const QByteArray systemPing(uint32_t id) const override;
    const QByteArray systemDeviceInfo(uint32_t id) const override;

    QObject *decode(const QByteArray &buffer, QObject *parent = nullptr) const override;
};


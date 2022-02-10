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

    const QByteArray systemDeviceInfo(uint32_t commandID) const override;

    QObject *decode(const QByteArray &buffer) const override;
};


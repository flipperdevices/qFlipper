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
    const QString testMethod() const override;
};


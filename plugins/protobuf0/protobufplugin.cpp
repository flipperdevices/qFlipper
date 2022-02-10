#include "protobufplugin.h"

#include "systemresponse.h"

ProtobufPlugin::ProtobufPlugin(QObject *parent):
    QObject(parent)
{}

const QByteArray ProtobufPlugin::systemDeviceInfo(uint32_t commandID) const
{
    return QByteArray::number(commandID, 16);
}

QObject *ProtobufPlugin::decode(const QByteArray &buffer) const
{
    Q_UNUSED(buffer)
    return new SystemPingResponse;
}

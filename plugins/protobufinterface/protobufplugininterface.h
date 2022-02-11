#pragma once

#include <QtPlugin>
#include <QByteArray>

#include "mainresponseinterface.h"

class ProtobufPluginInterface
{
public:
    virtual ~ProtobufPluginInterface() {}

    virtual const QByteArray testSystemPingResponse() const = 0;

    virtual const QByteArray systemPing(uint32_t id) const = 0;
    virtual const QByteArray systemDeviceInfo(uint32_t id) const = 0;

    virtual QObject *decode(const QByteArray &buffer, QObject *parent = nullptr) const = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(ProtobufPluginInterface, "com.flipperdevices.ProtobufPluginInterface/1.0")
QT_END_NAMESPACE

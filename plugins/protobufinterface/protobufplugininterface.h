#pragma once

#include <QtPlugin>

#include <QString>
#include <QByteArray>

class AbstractProtobufMessage
{
public:
    virtual ~AbstractProtobufMessage() {}
    virtual uint32_t commandID() const = 0;
    virtual bool hasNext() const = 0;
};

class ProtobufPluginInterface
{
public:
    virtual ~ProtobufPluginInterface() {}

    virtual const QByteArray systemDeviceInfo(uint32_t commandID) const = 0;

    virtual AbstractProtobufMessage *decode(const QByteArray &buffer) const = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(ProtobufPluginInterface, "com.flipperdevices.ProtobufPluginInterface/1.0")
QT_END_NAMESPACE

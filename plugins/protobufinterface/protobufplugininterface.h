#pragma once

#include <QtPlugin>

#include <QString>
#include <QByteArray>

class ProtobufPluginInterface
{
public:
    virtual ~ProtobufPluginInterface() {}
    virtual const QString testMethod() const = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(ProtobufPluginInterface, "com.flipperdevices.ProtobufPluginInterface/1.0")
QT_END_NAMESPACE

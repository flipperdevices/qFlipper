#include "protobufplugin.h"

ProtobufPlugin::ProtobufPlugin(QObject *parent):
    QObject(parent)
{}

const QString ProtobufPlugin::testMethod() const
{
    return QStringLiteral("Hello there!");
}

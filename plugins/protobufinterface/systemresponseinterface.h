#pragma once

#include <QtPlugin>
#include <QDateTime>
#include <QByteArray>

class SystemPingResponseInterface
{
public:
    virtual const QByteArray data() const = 0;
};

class SystemDeviceInfoResponseInterface
{
public:
    virtual const QByteArray key() const = 0;
    virtual const QByteArray value() const = 0;
};

class SystemDateTimeResponseInterface
{
public:
    virtual const QDateTime dateTime() const = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(SystemPingResponseInterface, "com.flipperdevices.SystemPingResponseInterface/1.0")
Q_DECLARE_INTERFACE(SystemDeviceInfoResponseInterface, "com.flipperdevices.SystemDeviceInfoResponseInterface/1.0")
Q_DECLARE_INTERFACE(SystemDateTimeResponseInterface, "com.flipperdevices.SystemDateTimeResponseInterface/1.0")
QT_END_NAMESPACE

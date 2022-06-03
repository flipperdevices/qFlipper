#pragma once

#include <QtPlugin>
#include <QDateTime>
#include <QByteArray>

class SystemDeviceInfoResponseInterface
{
public:
    virtual const QByteArray key() const = 0;
    virtual const QByteArray value() const = 0;
};

class SystemGetDateTimeResponseInterface
{
public:
    virtual const QDateTime dateTime() const = 0;
};

class SystemUpdateResponseInterface
{
public:
    virtual bool isResultOk() const = 0;
    virtual const QString resultString() const = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(SystemDeviceInfoResponseInterface, "com.flipperdevices.SystemDeviceInfoResponseInterface/1.0")
Q_DECLARE_INTERFACE(SystemGetDateTimeResponseInterface, "com.flipperdevices.SystemGetDateTimeResponseInterface/1.0")
Q_DECLARE_INTERFACE(SystemUpdateResponseInterface, "com.flipperdevices.SystemUpdateResponseInterface/1.0")
QT_END_NAMESPACE

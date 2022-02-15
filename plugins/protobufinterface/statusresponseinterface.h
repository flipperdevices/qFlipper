#pragma once

#include <QtPlugin>
#include <QByteArray>

class StatusPingResponseInterface
{
public:
    virtual const QByteArray data() const = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(StatusPingResponseInterface, "com.flipperdevices.StatusPingResponseInterface/1.0")
QT_END_NAMESPACE

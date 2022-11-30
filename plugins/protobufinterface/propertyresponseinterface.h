#pragma once

#include <QtPlugin>
#include <QByteArray>

class PropertyGetResponseInterface
{
public:
    virtual const QByteArray key() const = 0;
    virtual const QByteArray value() const = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(PropertyGetResponseInterface, "com.flipperdevices.PropertyGetResponseInterface/1.0")
QT_END_NAMESPACE

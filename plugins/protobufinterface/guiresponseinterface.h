#pragma once

#include <QtPlugin>
#include <QByteArray>

class GuiScreenFrameResponseInterface
{
public:
    virtual const QByteArray screenFrame() const = 0;
    virtual Qt::ScreenOrientation screenOrientation() const = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GuiScreenFrameResponseInterface, "com.flipperdevices.GuiScreenFrameResponseInterface/1.0")
QT_END_NAMESPACE

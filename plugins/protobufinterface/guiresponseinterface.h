#pragma once

#include <QtPlugin>
#include <QByteArray>

class GuiScreenFrameResponseInterface
{
public:
    enum ScreenOrientation {
        Horizontal,
        HorizontalFlip,
        Vertical,
        VerticalFlip,
    };

    virtual const QByteArray screenFrame() const = 0;
    virtual ScreenOrientation screenOrientation() const = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GuiScreenFrameResponseInterface, "com.flipperdevices.GuiScreenFrameResponseInterface/1.0")
QT_END_NAMESPACE

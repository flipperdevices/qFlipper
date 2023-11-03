#pragma once

#include <QMetaType>
#include <QByteArray>
#include <QSize>

struct ScreenFrame {
    QByteArray pixelData;
    QSize size;
    Qt::ScreenOrientation orientation;
};

Q_DECLARE_METATYPE(ScreenFrame)

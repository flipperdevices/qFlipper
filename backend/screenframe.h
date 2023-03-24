#pragma once

#include <QMetaType>
#include <QByteArray>
#include <QSize>

struct ScreenFrame {
    QByteArray pixelData;
    QSize size;
    bool isFlipped;
};

Q_DECLARE_METATYPE(ScreenFrame)

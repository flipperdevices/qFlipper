#include "screencanvas.h"

#include <QDebug>
#include <QPainter>

#define CANVAS_WIDTH 128
#define CANVAS_HEIGHT 64

ScreenCanvas::ScreenCanvas(QQuickItem *parent):
    QQuickPaintedItem(parent),
    m_fg(QColor(0x00, 0x00, 0x00)),
    m_bg(QColor(0xFF, 0x8B, 0x29)),
    m_canvas(QImage(CANVAS_WIDTH, CANVAS_HEIGHT, QImage::Format_RGB32))
{}

const QByteArray &ScreenCanvas::data() const
{
    static QByteArray dummy;
    return dummy;
}

void ScreenCanvas::setData(const QByteArray &data)
{
    for (size_t x=0; x < CANVAS_WIDTH; x++) {
        for (size_t y=0; y < CANVAS_HEIGHT; y++) {
            auto i = y / 8 * 128 + x;
            auto z = y & 7;
            auto color = ((data.at(i) & (1 << z))) ? m_fg : m_bg;
            m_canvas.setPixelColor(x, y, color);
        }
    }

    update();
}

void ScreenCanvas::paint(QPainter *painter)
{
    painter->drawImage(this->boundingRect(), m_canvas);
}

#include "screencanvas.h"

#include <cmath>
#include <QPainter>

#include "macros.h"

ScreenCanvas::ScreenCanvas(QQuickItem *parent):
    QQuickPaintedItem(parent),
    m_foreground(QColor(0x00, 0x00, 0x00)),
    m_background(QColor(0xFF, 0x8B, 0x29)),
    m_canvas(QImage(1, 1, QImage::Format_RGB32))
{}

const QByteArray &ScreenCanvas::data() const
{
    static QByteArray dummy;
    return dummy;
}

void ScreenCanvas::setData(const QByteArray &data)
{
    for (auto x = 0; x < canvasWidth(); x++) {
        for (auto y = 0; y < canvasHeight(); y++) {
            const auto i = y / 8 * canvasWidth() + x;
            const auto z = y & 7;
            const auto color = ((data.at(i) & (1 << z))) ? m_foreground : m_background;
            m_canvas.setPixelColor(x, y, color);
        }
    }

    update();
}

void ScreenCanvas::paint(QPainter *painter)
{
    const auto bw = boundingRect().width();
    const auto bh = boundingRect().height();

    const auto aspectRatio = (double)canvasWidth() / canvasHeight();

    auto w = bw;
    auto h = floor(w / aspectRatio);

    if(h > bh) {
        h = bh;
        w = bh * aspectRatio;
    }

    w -= ((int)w % (int)canvasWidth());
    h -= ((int)h % (int)canvasHeight());

    setRenderWidth(w);
    setRenderHeight(h);

    const auto dw = (bw - w) / 2;
    const auto dh = (bh - h) / 2;

    const QRectF canvasRect(dw, dh, w, h);
    painter->setPen(QColor(0x2E, 0x2E, 0x2E));
    painter->drawImage(canvasRect, m_canvas);
    painter->drawRoundedRect(canvasRect.adjusted(-1, -1, 1, 1), 4, 4);
}

qreal ScreenCanvas::canvasWidth() const
{
    return m_canvas.width();
}

void ScreenCanvas::setCanvasWidth(qreal w)
{
    if (qFuzzyCompare(canvasWidth(), w)) {
        return;
    }

    m_canvas = QImage(w, canvasHeight(), QImage::Format_RGB32);
    m_canvas.fill(m_background);
    emit canvasWidthChanged();

    update();
}

qreal ScreenCanvas::canvasHeight() const
{
    return m_canvas.height();
}

void ScreenCanvas::setCanvasHeight(qreal h)
{
    if(qFuzzyCompare(canvasHeight(), h)) {
        return;
    }

    m_canvas = QImage(canvasWidth(), h, QImage::Format_RGB32);
    m_canvas.fill(m_background);
    emit canvasHeightChanged();

    update();
}

qreal ScreenCanvas::renderWidth() const
{
    return m_renderWidth;
}

qreal ScreenCanvas::renderHeight() const
{
    return m_renderHeight;
}

void ScreenCanvas::saveImage(const QUrl &url)
{
    check_return_void(m_canvas.save(url.toLocalFile()), "Failed to save image");
}

void ScreenCanvas::setRenderWidth(qreal w)
{
    if(qFuzzyCompare(m_renderWidth, w)) {
        return;
    }

    m_renderWidth = w;
    emit renderWidthChanged();
}

void ScreenCanvas::setRenderHeight(qreal h)
{
    if(qFuzzyCompare(m_renderHeight, h)) {
        return;
    }

    m_renderHeight = h;
    emit renderHeightChanged();
}

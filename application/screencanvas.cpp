#include "screencanvas.h"

#include <cmath>
#include <QPainter>
#include <QClipboard>
#include <QGuiApplication>

#include "debug.h"

ScreenCanvas::ScreenCanvas(QQuickItem *parent):
    QQuickPaintedItem(parent),
    m_foreground(QColor(0x00, 0x00, 0x00)),
    m_background(QColor(0xFF, 0xFF, 0xFF)),
    m_canvas(QImage(1, 1, QImage::Format_RGB32))
{}

const QByteArray &ScreenCanvas::data() const
{
    static QByteArray dummy;
    return dummy;
}

void ScreenCanvas::setData(const QByteArray &data)
{
    if(data.isEmpty()) {
        return;
    }

    for (auto x = 0; x < canvasWidth(); x++) {
        for (auto y = 0; y < canvasHeight(); y++) {
            const auto i = y / 8 * canvasWidth() + x;
            const auto z = y % 8;
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
    painter->drawImage(canvasRect, m_canvas);
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

const QColor &ScreenCanvas::foregroundColor() const
{
    return m_foreground;
}

void ScreenCanvas::setForegroundColor(const QColor &color)
{
    if(color == m_foreground) {
        return;
    }

    m_foreground = color;
    emit foregroundColorChanged();
}

const QColor &ScreenCanvas::backgroundColor() const
{
    return m_background;
}

void ScreenCanvas::setBackgroundColor(const QColor &color)
{
    if(color == m_background) {
        return;
    }

    m_background = color;
    emit backgroundColorChanged();
}

void ScreenCanvas::saveImage(const QUrl &url, int scale)
{
    check_return_void(canvas(scale).save(url.toLocalFile()), "Failed to save image");
}

void ScreenCanvas::copyToClipboard(int scale)
{
    qGuiApp->clipboard()->setImage(canvas(scale));
}

void ScreenCanvas::setRenderWidth(qreal w)
{
    if(qFuzzyCompare(m_renderWidth, w)) {
        return;
    }

    m_renderWidth = w;
    emit renderWidthChanged();
}

const QImage ScreenCanvas::canvas(int scale) const
{
    if(scale == 0) {
        return m_canvas.scaled(m_renderWidth, m_renderHeight);
    } else {
        return m_canvas.scaled(canvasWidth() * scale, canvasHeight() * scale);
    }
}

void ScreenCanvas::setRenderHeight(qreal h)
{
    if(qFuzzyCompare(m_renderHeight, h)) {
        return;
    }

    m_renderHeight = h;
    emit renderHeightChanged();
}

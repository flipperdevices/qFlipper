#include "screencanvas.h"

#include <cmath>

#include <QPainter>
#include <QClipboard>
#include <QGuiApplication>

ScreenCanvas::ScreenCanvas(QQuickItem *parent):
    QQuickPaintedItem(parent),
    m_foreground(QColor(0x00, 0x00, 0x00)),
    m_background(QColor(0xFF, 0xFF, 0xFF)),
    m_canvas(QImage(1, 1, QImage::Format_RGB32)),
    m_zoomFactor(1.0)
{
    connect(this, &ScreenCanvas::zoomFactorChanged, this, &ScreenCanvas::updateImplicitSize);
    connect(this, &ScreenCanvas::canvasSizeChanged, this, &ScreenCanvas::updateImplicitSize);
}

const ScreenFrame &ScreenCanvas::frame() const
{
    static const ScreenFrame dummy {};
    return dummy;
}

void ScreenCanvas::setFrame(const ScreenFrame &frame)
{
    if(frame.pixelData.isEmpty() || frame.size.isEmpty()) {
        return;
    }

    setCanvasSize(frame.size);

    for (auto x = 0; x < m_canvas.width(); x++) {
        for (auto y = 0; y < m_canvas.height(); y++) {
            const auto i = y / 8 * m_canvas.width() + x;
            const auto z = y % 8;
            const auto color = ((frame.pixelData.at(i) & (1 << z))) ? m_foreground : m_background;

            m_canvas.setPixelColor(frame.isFlipped ? m_canvas.width() - x - 1 : x,
                                   frame.isFlipped ? m_canvas.height() - y - 1 : y,
                                   color);
        }
    }

    update();
}

void ScreenCanvas::paint(QPainter *painter)
{
    const auto bw = boundingRect().width();
    const auto bh = boundingRect().height();

    const auto aspectRatio = (double)m_canvas.width() / m_canvas.height();

    auto w = bw;
    auto h = floor(w / aspectRatio);

    if(h > bh) {
        h = bh;
        w = bh * aspectRatio;
    }

    w -= ((int)w % m_canvas.width());
    h -= ((int)h % m_canvas.height());

    const auto dw = (bw - w) / 2;
    const auto dh = (bh - h) / 2;

    const QRectF canvasRect(dw, dh, w, h);
    painter->drawImage(canvasRect, m_canvas);
}

qreal ScreenCanvas::zoomFactor() const
{
    return m_zoomFactor;
}

void ScreenCanvas::setZoomFactor(qreal zoom)
{
    if(qFuzzyCompare(m_zoomFactor, zoom)) {
        return;
    }

    m_zoomFactor = zoom;
    emit zoomFactorChanged();

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

bool ScreenCanvas::saveImage(const QUrl &url, int scale)
{
    return canvas(scale).save(url.toLocalFile());
}

void ScreenCanvas::copyToClipboard(int scale)
{
    qGuiApp->clipboard()->setImage(canvas(scale));
}

void ScreenCanvas::updateImplicitSize()
{
    setImplicitSize(m_zoomFactor * m_canvas.width(), m_zoomFactor * m_canvas.height());
}

void ScreenCanvas::setCanvasSize(const QSize &size)
{
    if(size == m_canvas.size()) {
        return;
    }

    m_canvas = QImage(size, QImage::Format_RGB32);
    m_canvas.fill(m_background);

    emit canvasSizeChanged();
}

const QImage ScreenCanvas::canvas(int scale) const
{
    if(scale == 0) {
        return m_canvas.scaled(m_canvas.width() * m_zoomFactor, m_canvas.height() * m_zoomFactor);
    } else {
        return m_canvas.scaled(m_canvas.width() * scale, m_canvas.height() * scale);
    }
}

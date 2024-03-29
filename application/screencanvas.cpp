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
    m_zoomFactor(1.0),
    m_orientation(Qt::LandscapeOrientation)
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
    setCanvasOrientation(frame.orientation);

    for (auto x = 0; x < m_canvas.width(); x++) {
        for (auto y = 0; y < m_canvas.height(); y++) {
            const auto i = y / 8 * m_canvas.width() + x;
            const auto z = y % 8;
            const auto color = ((frame.pixelData.at(i) & (1 << z))) ? m_foreground : m_background;

            m_canvas.setPixelColor(x, y, color);
        }
    }

    update();
}

void ScreenCanvas::paint(QPainter *painter)
{

    painter->drawImage(canvasRect(), m_canvas.transformed(canvasTransform()));
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

void ScreenCanvas::setCanvasOrientation(Qt::ScreenOrientation orientation)
{
    m_orientation = orientation;
}

QTransform ScreenCanvas::canvasTransform() const
{
    switch (m_orientation) {
    case Qt::InvertedLandscapeOrientation:
        return QTransform().rotate(180);
    case Qt::PortraitOrientation:
        return QTransform().rotate(isLandscapeOnly() ? 0 : 90);
    case Qt::InvertedPortraitOrientation:
        return QTransform().rotate(isLandscapeOnly() ? 180 : -90);
    case Qt::LandscapeOrientation:
    default:
        return QTransform();
    }
}

QRectF ScreenCanvas::canvasRect() const
{
    const auto totalWidth = boundingRect().width();
    const auto totalHeight = boundingRect().height();

    const auto isRegular = isLandscapeOrientation() || isLandscapeOnly();

    const auto canvasWidth = isRegular ? m_canvas.width() : m_canvas.height();
    const auto canvasHeight = isRegular ? m_canvas.height() : m_canvas.width();

    const auto aspectRatio = static_cast<qreal>(canvasWidth) / canvasHeight;

    auto drawWidth = totalWidth;
    auto drawHeight = floor(drawWidth / aspectRatio);

    if(drawHeight > totalHeight) {
        drawHeight = totalHeight;
        drawWidth = totalHeight * aspectRatio;
    }

    if(drawWidth > canvasWidth) {
        drawWidth -= (static_cast<int>(drawWidth) % canvasWidth);
    }

    if(drawHeight > canvasHeight) {
        drawHeight -= (static_cast<int>(drawHeight) % canvasHeight);
    }

    const auto offsetX = (totalWidth - drawWidth) / 2;
    const auto offsetY = (totalHeight - drawHeight) / 2;

    return QRectF(offsetX, offsetY, drawWidth, drawHeight);
}

QSize ScreenCanvas::canvasSize() const
{
    return QSize(isLandscapeOrientation() ? m_canvas.width() : m_canvas.height(),
                 isLandscapeOrientation() ? m_canvas.height() : m_canvas.width());
}

bool ScreenCanvas::isLandscapeOrientation() const
{
    return m_orientation == Qt::LandscapeOrientation ||
           m_orientation == Qt::InvertedLandscapeOrientation;
}

bool ScreenCanvas::isLandscapeOnly() const
{
    return (boundingRect().width() < canvasSize().width()) ||
           (boundingRect().height() < canvasSize().height());
}

const QImage ScreenCanvas::canvas(int scale) const
{
    if(scale == 0) {
        return m_canvas.scaled(m_canvas.width() * m_zoomFactor, m_canvas.height() * m_zoomFactor).transformed(canvasTransform());
    } else {
        return m_canvas.scaled(m_canvas.width() * scale, m_canvas.height() * scale).transformed(canvasTransform());
    }
}

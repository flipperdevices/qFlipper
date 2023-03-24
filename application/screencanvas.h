#pragma once

#include <QColor>
#include <QImage>
#include <QByteArray>
#include <QQuickPaintedItem>

#include "screenframe.h"

class ScreenCanvas : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(qreal zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)
    Q_PROPERTY(QColor foregroundColor READ foregroundColor WRITE setForegroundColor NOTIFY foregroundColorChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(ScreenFrame frame READ frame WRITE setFrame NOTIFY frameChanged)

public:
    ScreenCanvas(QQuickItem *parent = nullptr);

    const ScreenFrame &frame() const;
    void setFrame(const ScreenFrame &frame);

    void paint(QPainter *painter) override;

    qreal zoomFactor() const;
    void setZoomFactor(qreal zoom);

    const QColor &foregroundColor() const;
    void setForegroundColor(const QColor &color);

    const QColor &backgroundColor() const;
    void setBackgroundColor(const QColor &color);

    Q_INVOKABLE bool saveImage(const QUrl &url, int scale = 0);
    Q_INVOKABLE void copyToClipboard(int scale = 0);

signals:
    void zoomFactorChanged();
    void canvasSizeChanged();

    void foregroundColorChanged();
    void backgroundColorChanged();

    void frameChanged();

private slots:
    void updateImplicitSize();

private:
    void setCanvasSize(const QSize &size);

const QImage canvas(int scale = 0) const;

    QColor m_foreground;
    QColor m_background;
    QImage m_canvas;

    qreal m_zoomFactor;
};

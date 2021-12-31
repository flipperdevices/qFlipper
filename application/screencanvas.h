#pragma once

#include <QColor>
#include <QImage>
#include <QByteArray>
#include <QQuickPaintedItem>

class ScreenCanvas : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(qreal canvasWidth READ canvasWidth WRITE setCanvasWidth NOTIFY canvasWidthChanged)
    Q_PROPERTY(qreal canvasHeight READ canvasHeight WRITE setCanvasHeight NOTIFY canvasHeightChanged)
    Q_PROPERTY(qreal renderWidth READ renderWidth NOTIFY renderWidthChanged)
    Q_PROPERTY(qreal renderHeight READ renderHeight NOTIFY renderHeightChanged)
    Q_PROPERTY(QColor foregroundColor READ foregroundColor WRITE setForegroundColor NOTIFY foregroundColorChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(QByteArray data READ data WRITE setData)

public:
    ScreenCanvas(QQuickItem *parent = nullptr);

    const QByteArray &data() const;
    void setData(const QByteArray &data);

    void paint(QPainter *painter) override;

    qreal canvasWidth() const;
    void setCanvasWidth(qreal w);

    qreal canvasHeight() const;
    void setCanvasHeight(qreal h);

    qreal renderWidth() const;
    qreal renderHeight() const;

    const QColor &foregroundColor() const;
    void setForegroundColor(const QColor &color);

    const QColor &backgroundColor() const;
    void setBackgroundColor(const QColor &color);

public slots:
    void saveImage(const QUrl &url, int scale = 0);
    void copyToClipboard(int scale = 0);

signals:
    void canvasWidthChanged();
    void canvasHeightChanged();

    void renderWidthChanged();
    void renderHeightChanged();

    void foregroundColorChanged();
    void backgroundColorChanged();

private:
    void setRenderHeight(qreal h);
    void setRenderWidth(qreal w);

const QImage canvas(int scale = 0) const;

    QColor m_foreground;
    QColor m_background;
    QImage m_canvas;

    qreal m_renderWidth;
    qreal m_renderHeight;
};

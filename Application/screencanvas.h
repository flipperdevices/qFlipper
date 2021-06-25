#ifndef SCREENCANVAS_H
#define SCREENCANVAS_H

#include <QColor>
#include <QImage>
#include <QByteArray>
#include <QQuickPaintedItem>

class ScreenCanvas : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QByteArray data READ data WRITE setData)

public:
    ScreenCanvas(QQuickItem *parent = nullptr);

    const QByteArray &data() const;
    void setData(const QByteArray &data);

    void paint(QPainter *painter) override;

private:
    const QColor m_fg;
    const QColor m_bg;
    QImage m_canvas;
};

#endif // SCREENCANVAS_H

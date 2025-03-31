#ifndef IMAGEVIEWPORT_H
#define IMAGEVIEWPORT_H

#include <QEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QWheelEvent>

class ImageViewport : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ImageViewport(QWidget *parent = nullptr);

    void updateImage(const QImage &image);

    void fitToViewport();
    void originalSize();

signals:
    void scaleChanged(double scale);
    void mousePressed();
    void mouseDblPressed();
    void mouseReleased();

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QGraphicsScene *scene;
    QGraphicsPixmapItem *pixmapItem;
    double scaleFactor = 1.15;
    double scaleCurrent = 1.0;
};

#endif  // !IMAGEVIEWPORT_H

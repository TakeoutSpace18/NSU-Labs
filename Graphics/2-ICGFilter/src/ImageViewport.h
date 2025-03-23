#ifndef IMAGEVIEWPORT_H
#define IMAGEVIEWPORT_H

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>

class ImageViewport : public QGraphicsView {
    Q_OBJECT

public:
    explicit ImageViewport(QWidget *parent = nullptr);

    void updateImage(const QImage &image);
    void fitToViewport();
    void originalSize();

signals:
    void scaleChanged(double scale);

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    QGraphicsScene *scene;
    QGraphicsPixmapItem *pixmapItem;
    double scaleFactor = 1.15;
    double scaleCurrent = 1.0;
};

#endif // !IMAGEVIEWPORT_H

#include "ImageViewport.h"
#include <qnamespace.h>

#include <QGraphicsItem>
#include <QGraphicsScene>

ImageViewport::ImageViewport(QWidget *parent)
    : QGraphicsView(parent),
      scene(new QGraphicsScene(this)),
      pixmapItem(new QGraphicsPixmapItem())
{
    scene->addItem(pixmapItem);
    setScene(scene);

    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
}

void ImageViewport::updateImage(const QImage &image)
{
    Q_ASSERT(!image.isNull());

    pixmapItem->setPixmap(QPixmap::fromImage(image));
    scene->setSceneRect(pixmapItem->boundingRect());
    setSceneRect(scene->sceneRect());
}

void ImageViewport::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0) {
        scale(scaleFactor, scaleFactor);
        scaleCurrent *= scaleFactor;
    } else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        scaleCurrent /= scaleFactor;
    }

    emit scaleChanged(scaleCurrent);
}

void ImageViewport::fitToViewport()
{
    fitInView(pixmapItem->boundingRect(), Qt::KeepAspectRatio);
}

void ImageViewport::originalSize()
{
    resetTransform();
    scaleCurrent = 1.0;
}

void ImageViewport::mousePressEvent(QMouseEvent *event)
{
    emit mousePressed();
    QGraphicsView::mousePressEvent(event);
}

void ImageViewport::mouseReleaseEvent(QMouseEvent *event)
{
    emit mouseReleased();
    QGraphicsView::mouseReleaseEvent(event);
}

void ImageViewport::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit mouseDblPressed();
    QGraphicsView::mouseDoubleClickEvent(event);
}

void ImageViewport::setTransformationMode(Qt::TransformationMode mode)
{
    pixmapItem->setTransformationMode(mode);
}

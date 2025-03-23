#include "ImageViewport.h"
#include <qgraphicsitem.h>
#include <qgraphicsscene.h>
#include <qnamespace.h>

ImageViewport::ImageViewport(QWidget *parent)
    : QGraphicsView(parent),
    scene(new QGraphicsScene(this)),
    pixmapItem(new QGraphicsPixmapItem())
{
    scene->addItem(pixmapItem);
    setScene(scene);

    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setDragMode(QGraphicsView::ScrollHandDrag);
}

void ImageViewport::updateImage(const QImage& image)
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
    }
    else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        scaleCurrent /= scaleFactor;
    }

    emit scaleChanged(scaleCurrent);
}

void ImageViewport::fitToViewport()
{
    fitInView(pixmapItem, Qt::KeepAspectRatio);
}

void ImageViewport::originalSize()
{
    resetTransform();
}

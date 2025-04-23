#include "SceneViewport.h"

#include <QGraphicsItem>
#include <QGraphicsScene>

SceneViewport::SceneViewport(QWidget *parent)
    : QGraphicsView(parent),
      scene(new QGraphicsScene(this)),
      pixmapItem(new QGraphicsPixmapItem())
{
    scene->addItem(pixmapItem);
    setScene(scene);

    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
}

void SceneViewport::updateImage(const QImage &image)
{
    Q_ASSERT(!image.isNull());

    pixmapItem->setPixmap(QPixmap::fromImage(image));
    scene->setSceneRect(pixmapItem->boundingRect());
    setSceneRect(scene->sceneRect());
}

void SceneViewport::wheelEvent(QWheelEvent *event)
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

void SceneViewport::fitToViewport()
{
    fitInView(pixmapItem->boundingRect(), Qt::KeepAspectRatio);
}

void SceneViewport::originalSize()
{
    resetTransform();
    scaleCurrent = 1.0;
}

void SceneViewport::mousePressEvent(QMouseEvent *event)
{
    emit mousePressed();
    QGraphicsView::mousePressEvent(event);
}

void SceneViewport::mouseReleaseEvent(QMouseEvent *event)
{
    emit mouseReleased();
    QGraphicsView::mouseReleaseEvent(event);
}

void SceneViewport::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit mouseDblPressed();
    QGraphicsView::mouseDoubleClickEvent(event);
}

void SceneViewport::setTransformationMode(Qt::TransformationMode mode)
{
    pixmapItem->setTransformationMode(mode);
}

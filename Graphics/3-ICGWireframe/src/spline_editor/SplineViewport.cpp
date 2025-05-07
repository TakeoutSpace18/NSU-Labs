#include "SplineViewport.h"

#include <QTransform>

SplineViewport::SplineViewport(QWidget* parent) : QGraphicsView(parent)
{
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag);
}

void SplineViewport::wheelEvent(QWheelEvent* event)
{
    scaleView(pow(2., event->angleDelta().y() / 240.0));
}

void SplineViewport::scaleView(qreal scaleFactor)
{
    qreal factor = transform()
                       .scale(scaleFactor, scaleFactor)
                       .mapRect(QRectF(0, 0, 1, 1))
                       .width();

    if (factor < 1 || factor > 30) {
        return;
    }

    scale(scaleFactor, scaleFactor);
}
void SplineViewport::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
        case Qt::Key_Plus:
            zoomIn();
            break;
        case Qt::Key_Minus:
            zoomOut();
            break;
        case Qt::Key_R:
            resetTransform();
            break;
        default:
            QGraphicsView::keyPressEvent(event);
            break;
    }
}

void SplineViewport::zoomIn()
{
    scaleView(qreal(1.2));
}

void SplineViewport::zoomOut()
{
    scaleView(1 / qreal(1.2));
}

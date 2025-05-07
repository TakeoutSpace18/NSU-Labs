#include "AnchorPointItem.h"

#include <qgraphicsitem.h>
#include <qvectornd.h>

#include <QGraphicsItem>
#include <QPen>

#include "SplineEditor.h"

AnchorPointItem::AnchorPointItem(QVector2D pos, int index,
                                 QGraphicsItem* parent)
    : QGraphicsEllipseItem(-3, -3, 6, 6, parent)
{
    setPos(pos.x(), pos.y());

    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsSelectable);

    setCacheMode(DeviceCoordinateCache);
}

void AnchorPointItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    update();
    QGraphicsEllipseItem::mousePressEvent(event);
}

// void AnchorPointItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
// {
//     update();
//     QGraphicsEllipseItem::mouseMoveEvent(event);
// }

void AnchorPointItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    update();
    QGraphicsEllipseItem::mouseReleaseEvent(event);
}

QPainterPath AnchorPointItem::shape() const
{
    QPainterPath path;
    path.addEllipse(-10, -10, 20, 20);
    return path;
}

void AnchorPointItem::paint(QPainter* painter,
                            const QStyleOptionGraphicsItem* option,
                            QWidget* widget)
{
    QRadialGradient gradient(0, 0, 10);
    if (isSelected()) {
        gradient.setCenter(0, 0);
        gradient.setFocalPoint(0, 0);
        gradient.setColorAt(1, QColor(Qt::yellow).lighter(120));
        gradient.setColorAt(0, QColor(Qt::darkYellow).lighter(120));
    }
    else {
        gradient.setColorAt(0, Qt::yellow);
        gradient.setColorAt(1, Qt::darkYellow);
    }
    painter->setBrush(gradient);

    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(-6, -6, 12, 12);
}

QVariant AnchorPointItem::itemChange(GraphicsItemChange change,
                                     const QVariant& value)
{
    if (scene()) {
        SplineEditor* editor = qobject_cast<SplineEditor*>(scene()->parent());

        if (change == ItemPositionHasChanged && editor) {
            editor->anchorPointMoved(this);
        }
        else if (change == ItemSelectedHasChanged && editor) {
            editor->anchorPointSelectChanged(this, value.toBool());
        }
    }

    return QGraphicsEllipseItem::itemChange(change, value);
}

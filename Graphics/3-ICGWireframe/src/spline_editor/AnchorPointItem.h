#ifndef ANCHORPOINTITEM_H
#define ANCHORPOINTITEM_H

#include <qstyleoption.h>
#include <qvectornd.h>

#include <QGraphicsItem>

class AnchorPointItem : public QGraphicsEllipseItem
{
public:
    AnchorPointItem(QVector2D pos, int index, QGraphicsItem *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
    QPainterPath shape() const override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    // void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value) override;

private:
    QPointF m_startPos;
};

#endif  // !ANCHORPOINTITEM_H

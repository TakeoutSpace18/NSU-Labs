#include "LineTool.h"

#include <QMouseEvent>
#include <QPainter>

LineTool::LineTool(Canvas *canvas, const QSharedPointer<const LineTool::Options>& options)
    : Tool(canvas), m_firstPoint(0, 0), m_firstPointSelected(false),
    m_options(options), m_preview(canvas->size())
{
}

void LineTool::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (!m_firstPointSelected) {
            m_firstPoint = event->pos();
            m_firstPointSelected = true;
            setCursor(Qt::CrossCursor);
        }
        else {
            m_canvas->drawLine(m_firstPoint, event->pos(),
                               m_options->color, m_options->width);
            m_firstPointSelected = false;
            setCursor(Qt::ArrowCursor);
        }
    }
}

void LineTool::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && m_firstPointSelected) {
        QPainter painter(this);
        painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap,
                            Qt::RoundJoin));
        painter.drawLine(m_firstPoint, event->pos());

        int rad = (10 / 2) + 2;
        update(QRect(m_firstPoint, event->pos()).normalized().adjusted(-rad, -rad, +rad, +rad));
    }
 }

QIcon LineTool::Icon()
{
    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawLine(3, 29, 29, 3);

    return QIcon(pixmap);
}


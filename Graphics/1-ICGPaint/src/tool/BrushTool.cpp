#include "BrushTool.h"

#include <QMouseEvent>
#include <QPainter>

BrushTool::BrushTool(Canvas *canvas, const QSharedPointer<const BrushTool::Options>& options)
    : Tool(canvas), m_lastPoint(0, 0), m_isPainting(false), m_options(options)
{

}

void BrushTool::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_lastPoint = event->pos();
        m_isPainting = true;
        setCursor(Qt::CrossCursor);
    }
}

void BrushTool::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && m_isPainting) {
        m_canvas->drawLine(m_lastPoint, event->pos(),
                           m_options->color, m_options->width);
        m_lastPoint = event->pos();
    }
 }

void BrushTool::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_isPainting) {
        m_canvas->drawLine(m_lastPoint, event->pos());
        m_isPainting = false;
        setCursor(Qt::ArrowCursor);
    }
}

QIcon BrushTool::Icon()
{
    return QIcon(":resources/icons/brush.svg");
}

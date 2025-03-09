#include "LineTool.h"

#include "LineToolOptionsPanel.h"
#include "ToolOptionsPanel.h"
#include "BresenhamLine.h"

#include <QMouseEvent>
#include <QPainter>

LineTool::LineTool(Canvas *canvas, const QSharedPointer<LineToolOptions>& options,
                   QWidget *parent)
    : Tool(canvas, parent),
    m_firstPoint(0, 0),
    m_firstPointSelected(false),
    m_options(options)
{}

void LineTool::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (!m_firstPointSelected) {
            m_firstPoint = event->pos();
            m_firstPointSelected = true;
            setCursor(Qt::CrossCursor);
        }
        else {
            clearPreview();

            m_canvas->drawLine(m_firstPoint, event->pos(),
                               m_options->color(), m_options->width());
            m_firstPointSelected = false;
            setCursor(Qt::ArrowCursor);
        }
    }
}

void LineTool::mouseMoveEvent(QMouseEvent *event)
{
    if (m_firstPointSelected) {
        drawPreview(m_firstPoint, event->pos());
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

void LineTool::drawPreview(QPoint a, QPoint b)
{
    // TODO: do not refill entire image on each redraw
    m_preview.fill(Qt::transparent);
    BresenhamLine::Draw(m_preview, a, b, m_options->color(), m_options->width());
    update();
}


ToolOptionsPanel* LineTool::createOptionsPanel(QWidget* parent) const
{
    return new LineToolOptionsPanel(m_options, parent);
}

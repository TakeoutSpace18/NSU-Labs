#include "FillTool.h"

#include <QMouseEvent>
#include <QPainter>

FillTool::FillTool(Canvas *canvas, const QSharedPointer<const FillTool::Options>& options)
    : Tool(canvas), m_options(options)
{
}

void FillTool::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        doSpanFilling(event->pos());
    }
}

QIcon FillTool::Icon()
{
    return QIcon(":resources/icons/fill.svg");
}

void FillTool::doSpanFilling(QPoint seed)
{


}

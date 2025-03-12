#include "Tool.h"

#include <QResizeEvent>
#include <QPainter>
#include <qglobal.h>

Tool::Tool(Canvas *canvas, QWidget *parent)
    : QWidget(parent),
    m_canvas(canvas),
    m_preview(canvas->size(), QImage::Format_ARGB32)
{
    setGeometry(0, 0, canvas->width(), canvas->height());

    setMouseTracking(true);
    m_preview.fill(Qt::transparent);
}

void Tool::resizeEvent(QResizeEvent *event)
{
    if (m_preview.size() == event->size()) {
        return;
    }

    QImage newPreview(event->size(), QImage::Format_ARGB32);
    newPreview.fill(Qt::transparent);

    m_preview = newPreview;

    update();

    QWidget::resizeEvent(event);
}

void Tool::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, m_preview, dirtyRect);
}

void Tool::clearPreview()
{
    m_preview.fill(Qt::transparent);
    update();
}

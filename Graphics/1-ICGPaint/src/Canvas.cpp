#include "Canvas.h"

#include <QMouseEvent>
#include <QPainter>
#include <QHBoxLayout>
#include <QImage>

#include "BresenhamLine.h"

Canvas::Canvas(const QSize& size, QWidget *parent)
    : Canvas(QImage(size, QImage::Format_ARGB32))
{
    clear();
}

Canvas::Canvas(const QImage& image, QWidget *parent)
    : QWidget(parent), m_image(image)
{
    m_image.convertTo(QImage::Format_ARGB32);

    setAttribute(Qt::WA_StaticContents);
    resize(m_image.size());

    update();
}
void Canvas::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, m_image, dirtyRect);
}

void Canvas::drawLine(QPoint a, QPoint b, QColor color, int width)
{
    BresenhamLine::Draw(m_image, a, b, color, width);

    int rad = (width / 2) + 2;
    update(QRect(a, b).normalized().adjusted(-rad, -rad, +rad, +rad));
}

void Canvas::resizeEvent(QResizeEvent *event)
{
    if (m_image.size() == event->size()) {
        return;
    }

    QImage newImage(event->size(), QImage::Format_RGB32);
    newImage.fill(Qt::white);
    QPainter painter(&newImage);
    qDebug() << m_image.isNull();
    painter.drawImage(QPoint(0, 0), m_image);

    m_image = newImage;

    update();

    QWidget::resizeEvent(event);
}

void Canvas::clear()
{
    m_image.fill(Qt::white);
    update();
}

void Canvas::setImage(const QImage& image)
{
    m_image = image;
    m_image.convertTo(QImage::Format_ARGB32);
    resize(image.size());
    update();
}


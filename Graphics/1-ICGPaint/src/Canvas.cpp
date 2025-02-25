#include "Canvas.h"
#include "BresenhamLine.h"

#include <QMouseEvent>
#include <QPainter>
#include <QHBoxLayout>

Canvas::Canvas(const QSize& size, QWidget *parent) : QWidget(),
    m_image(size, QImage::Format_RGB32)
{
    // only for optimization
    setAttribute(Qt::WA_StaticContents);
    resize(m_image.size());
    clear();

    BresenhamLine::Test(m_image);
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
    qDebug() << "Canvas resize " << event->size();

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

void Canvas::setPixelColor(int x, int y, const QColor& color)
{
    m_image.setPixelColor(x, y, color);
}

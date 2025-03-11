#include "StampTool.h"

#include "StampToolOptions.h"
#include "StampToolOptionsPanel.h"
#include "ToolOptionsPanel.h"
#include "BresenhamLine.h"

#include <QMouseEvent>
#include <QPainter>
#include <cmath>
#include <qglobal.h>
#include <qnamespace.h>
#include <qpoint.h>

StampTool::StampTool(Canvas *canvas, const QSharedPointer<StampToolOptions>& options,
                   QWidget *parent)
    : Tool(canvas, parent),
    m_options(options)
{}

void StampTool::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        Draw(m_canvas->image(), event->pos(), *m_options);
    }
}

void StampTool::mouseMoveEvent(QMouseEvent *event)
{
    drawPreview(event->pos());
 }

QIcon StampTool::Icon()
{
    StampToolOptions options;
    options.setShape(StampToolOptions::Shape::Polygon);
    options.setWidth(2);
    options.setColor(Qt::black);
    options.setRadius(14);
    options.setRotation(270);
    options.setVertices(5);

    return Icon(options);
}

QIcon StampTool::Icon(const StampToolOptions& options)
{
    // this is awful, but I didn't know that QObject can not be copied
    StampToolOptions resizedOptions;
    resizedOptions.setColor(options.color());
    resizedOptions.setWidth(options.width());
    resizedOptions.setRadius(14);
    resizedOptions.setInnerRadius(options.innerRadius() * 14 / options.radius());
    resizedOptions.setVertices(options.vertices());
    resizedOptions.setRotation(options.rotation());


    QImage image(32, 32, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    Draw(image, QPoint(image.size().width() / 2, image.size().height() / 2),
         resizedOptions);

    return QIcon(QPixmap::fromImage(image));
}

void StampTool::drawPreview(QPoint center)
{
    // TODO: do not refill entire image on each redraw
    m_preview.fill(Qt::transparent);
    Draw(m_preview, center, *m_options);
    update();
}

ToolOptionsPanel* StampTool::createOptionsPanel(QWidget* parent) const
{
    return new StampToolOptionsPanel(m_options, parent);
}

QVector<QPoint> StampTool::GeneratePolygonPoints(QPoint center, const StampToolOptions& options)
{
    QVector<QPoint> points;

    double angleStep = 2 * M_PI / options.vertices();
    double angle = options.rotation() * 2 * M_PI / 360;
    int radius = options.radius();

    for (int i = 0; i < options.vertices(); ++i) {
        points.append(center + QPoint(radius * std::cos(angle), radius * std::sin(angle)));
        angle += angleStep;
    }

    return points;
}

QVector<QPoint> StampTool::GenerateStarPoints(QPoint center, const StampToolOptions& options)
{
    QVector<QPoint> points;

    double angleStep = M_PI / options.vertices();
    double angle = options.rotation() * 2 * M_PI / 360;
    int radius = options.radius();
    int innerRadius = options.innerRadius();

    for (int i = 0; i < options.vertices(); ++i) {
        points.append(center + QPoint(radius * std::cos(angle), radius * std::sin(angle)));
        angle += angleStep;
        points.append(center + QPoint(innerRadius * std::cos(angle), innerRadius * std::sin(angle)));
        angle += angleStep;
    }

    return points;
}

void StampTool::Draw(QImage& image, QPoint center, const StampToolOptions& options)
{
    QVector<QPoint> points;
    if (options.shape() == StampToolOptions::Shape::Polygon) {
        points = GeneratePolygonPoints(center, options);
    }
    else if (options.shape() == StampToolOptions::Shape::Star) {
        points = GenerateStarPoints(center, options);
    }

    int i;
    for (i = 0; i < points.size() - 1; ++i) {
        BresenhamLine::Draw(image, points[i], points[i + 1],
                            options.color(), options.width());
    }
    BresenhamLine::Draw(image, points[i], points[0],
                        options.color(), options.width());
}

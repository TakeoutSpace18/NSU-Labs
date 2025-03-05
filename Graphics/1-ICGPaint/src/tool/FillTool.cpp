#include "FillTool.h"

#include <QMouseEvent>
#include <QPainter>
#include <QStack>

FillTool::FillTool(Canvas *canvas, const QSharedPointer<FillToolOptions>& options)
    : Tool(canvas), m_options(options)
{
}

void FillTool::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        doSpanFilling(m_canvas->image(), event->pos(), m_options->color());
    }
}

QIcon FillTool::Icon()
{
    return QIcon(":resources/icons/fill.svg");
}

void FillTool::doSpanFilling(QImage& image, QPoint seed, const QColor& color)
{
    QRgb *pixelData = reinterpret_cast<QRgb*>(image.bits());
    QRgb areaColor = pixelData[seed.y() * image.width() + seed.x()];

    if (areaColor == m_options->color().rgba()) {
        return;
    }

    auto inside = [&](int x, int y) -> bool {
        if (!image.rect().contains(x, y)) {
            return false;
        }
        return pixelData[y * image.width() + x] == areaColor;
    };

    auto scan = [&](int lx, int rx, int y, QStack<QPoint>& stack) {
        bool span_added = false;

        for (int x = lx; x < rx; ++x) {
            if (!inside(x, y)) {
                span_added = false;
            }
            else if (!span_added) {
                stack.push(QPoint(x, y));
                span_added = true;
            }
        }
    };

    QStack<QPoint> stack;
    stack.push(seed);

    while (!stack.empty()) {
        seed = stack.pop();
        int y = seed.y();

        int lx = seed.x();
        while (inside(lx - 1, y)) {
            pixelData[y * image.width() + (lx - 1)] = color.rgba();
            lx -= 1;
        }

        int rx = seed.x();
        while (inside(rx, y)) {
            pixelData[y * image.width() + rx] = color.rgba();
            rx += 1;
        }

        scan(lx, rx, y + 1, stack);
        scan(lx, rx, y - 1, stack);
    }

    m_canvas->update();
}

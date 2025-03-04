#include "BresenhamLine.h"

#include <cmath>
#include <QRgb>

void BresenhamLine::Draw(QImage& image, QPoint a, QPoint b, QColor color,
                    int width)
{
    Q_ASSERT(image.format() == QImage::Format_ARGB32);
    Q_ASSERT(image.rect().contains(a) && image.rect().contains(b));
    Q_ASSERT(width > 0);

    QRgb *pixelData = reinterpret_cast<QRgb*>(image.bits());

    auto setcolor = [&](int x, int y, const QColor& color) {
        if (image.rect().contains(x, y)) {
            pixelData[y * image.width() + x] = color.rgba();
        }
    };

    int dx = std::abs(b.x() - a.x());
    int dy = std::abs(b.y() - a.y());

    int stepX = a.x() < b.x() ? 1 : -1;
    int stepY = a.y() < b.y() ? 1 : -1;

    int y = a.y();
    int x = a.x();

    if (dy < dx)
    {
        int err = -2 * dy - dx;
        for (int i = 0; i < dx; ++i) {
            x += stepX;
            err += 2 * dy;
            if (err > 0) {
                err -= 2 * dx;
                y += stepY;
            }

            int off = width / 2;
            for (int i = 0; i < width; ++i) {
                setcolor(x, y - off + i, color);
            }
        }
    }
    else
    {
        int err = -2 * dx - dy;
        for (int i = 0; i < dy; ++i) {
            y += stepY;
            err += 2 * dx;
            if (err > 0) {
                err -= 2 * dy;
                x += stepX;
            }

            int off = width / 2;
            for (int i = 0; i < width; ++i) {
                setcolor(x - off + i, y, color);
            }
        }
    }

}

void BresenhamLine::Test(QImage& image)
{
    QPoint center(image.width() / 2, image.height() / 2);

    const double step = 0.1;
    const double length = std::min(image.width() / 2, image.height() / 2);

    for (double angle = 0; angle < 2 * M_PI; angle += step) {

        QPoint end(
            center.x() + std::cos(angle) * length,
            center.y() + std::sin(angle) * length
        );

        BresenhamLine::Draw(image, center, end, Qt::black, 5);
    }
}


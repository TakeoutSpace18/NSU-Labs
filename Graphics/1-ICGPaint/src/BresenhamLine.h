#ifndef BRESENHAM_LINE_H
#define BRESENHAM_LINE_H

#include <QImage>

class BresenhamLine
{
public:
    static void Draw(QImage& image, QPoint a, QPoint b,
                     QColor color = Qt::black, int width = 1);

    static void Test(QImage& image);

private:

};

#endif

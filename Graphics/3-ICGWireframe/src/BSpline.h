#ifndef BSPLINE_H
#define BSPLINE_H

#include <QVector>
#include <QVector2D>

class BSpline
{
public:
    static QVector<QVector2D> Generate(const QVector<QVector2D>& anchorPoints, qsizetype N);
};

#endif  // !BSPLINE

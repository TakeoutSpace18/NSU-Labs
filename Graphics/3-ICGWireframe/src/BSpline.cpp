#include "BSpline.h"

#include <QMatrix4x4>

QVector<QVector2D> BSpline::Generate(const QVector<QVector2D>& anchorPoints,
                                     qsizetype N)
{
    QVector<QVector2D> points;

    // clang-format off
    static QMatrix4x4 M = QMatrix4x4(-1, 3, -3, 1,
                                     3, -6, 3, 0,
                                     -3, 0, 3, 0,
                                     1, 4, 1, 0) / 6;
    // clang-format on

    for (int i = 1; i < anchorPoints.size() - 2; ++i) {
        // clang-format off
        QVector4D Gx(anchorPoints[i - 1].x(),
                     anchorPoints[i].x(),
                     anchorPoints[i + 1].x(),
                     anchorPoints[i + 2].x());

        QVector4D Gy(anchorPoints[i - 1].y(),
                     anchorPoints[i].y(),
                     anchorPoints[i + 1].y(),
                     anchorPoints[i + 2].y());
        // clang-format on

        for (int j = 0; j < N; ++j) {
            float t = j / static_cast<float>(N);

            QVector4D T(t * t * t, t * t, t, 1);

            float x = QVector4D::dotProduct(T, M * Gx);
            float y = QVector4D::dotProduct(T, M * Gy);
            points.push_back(QVector2D(x, y));
        }

        if (i == anchorPoints.size() - 3) {
            float t = 1.0;

            QVector4D T(t * t * t, t * t, t, 1);

            float x = QVector4D::dotProduct(T, M * Gx);
            float y = QVector4D::dotProduct(T, M * Gy);
            points.push_back(QVector2D(x, y));
        }
    }

    return points;
}

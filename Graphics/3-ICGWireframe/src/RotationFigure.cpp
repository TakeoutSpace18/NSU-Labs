#include "RotationFigure.h"

#include <qglobal.h>

#include <cmath>

#include "BSpline.h"
#include "SceneData.h"

QVector<QVector<QVector3D>> RotationFigure::Generate(const SceneData& sceneData)
{
    QVector<QVector<QVector3D>> shapes;

    QVector<QVector2D> generatixPoints = BSpline::Generate(
        sceneData.anchorPoints(), sceneData.generatrixSegmentsNumber());

    int genNum = sceneData.generatrixNumber();

    for (int i = 0; i < genNum; ++i) {
        QVector<QVector3D> shape;
        float phi = 2 * M_PI * i / genNum;
        for (QVector2D point : generatixPoints) {
            shape.append(QVector3D(point.y() * qCos(phi), point.y() * qSin(phi),
                                   point.x()));
        }

        shapes.append(qMove(shape));
    }

    QVector<QVector2D> anchors = sceneData.anchorPoints();

    int cirSig = sceneData.circleSegmentsNumber();
    int genSig = sceneData.generatrixSegmentsNumber();

    for (int i = 0; i < generatixPoints.size(); i += genSig) {
        QVector<QVector3D> shape;
        for (int j = 0; j < genNum; ++j) {
            for (int k = 0; k < cirSig; ++k) {
                float phi =
                    2 * M_PI * (j + static_cast<float>(k) / cirSig) / genNum;
                shape.append(QVector3D(generatixPoints[i].y() * qCos(phi),
                                       generatixPoints[i].y() * qSin(phi),
                                       generatixPoints[i].x()));
            }
        }

        shape.append(shape.first());
        shapes.append(qMove(shape));
    }

    return shapes;
}

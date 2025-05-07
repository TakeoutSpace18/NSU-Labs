#ifndef ROTATIONFIGURE_H
#define ROTATIONFIGURE_H

#include <QVector>
#include <QVector2D>

#include "SceneData.h"

class RotationFigure
{
public:
    static QVector<QVector<QVector3D>> Generate(const SceneData& sceneData);
};

#endif  // !BSPLINE

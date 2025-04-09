#include "RotationFilter.h"
#include <qglobal.h>

#include <QRgb>
#include <QTranslator>

RotationFilter::RotationFilter() : Filter("Rotation Filter")
{
    setParameter("angle", 0);
}

void RotationFilter::apply(const QImage &input, QImage &output) const
{
    Q_ASSERT(input.rect() == output.rect());

    const QRgb *inputPixels = reinterpret_cast<const QRgb *>(input.bits());

    int width = input.width();
    int height = input.height();

    int angleDegrees = parameters["angle"].toInt();

    double angleRad = qDegreesToRadians(angleDegrees);
    double cosA = qCos(angleRad);
    double sinA = qSin(angleRad);

    int newWidth = qCeil(qAbs(width * cosA) + qAbs(height * sinA));
    int newHeight = qCeil(qAbs(width * sinA) + qAbs(height * cosA));

    output = QImage(newWidth, newHeight, input.format());
    output.fill(Qt::white);

    QRgb *outputPixels = reinterpret_cast<QRgb *>(output.bits());

    QPointF inputCenter(width / 2.0, height / 2.0);
    QPointF outputCenter(newWidth / 2.0, newHeight / 2.0);

    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {

            double dx = x - outputCenter.x();
            double dy = y - outputCenter.y();

            double srcX =  cosA * dx + sinA * dy + inputCenter.x();
            double srcY = -sinA * dx + cosA * dy + inputCenter.y();

            int ix = qRound(srcX);
            int iy = qRound(srcY);

            if (ix >= 0 && ix < width && iy >= 0 && iy < height) {
                outputPixels[y * newWidth + x] = inputPixels[iy * width + ix];
            }
        }
    }
}

QList<Filter::ParameterInfo> RotationFilter::getParameterInfoList() const
{
    return {{"angle", "Angle", 0, -180, 180, "int", {}}};
}

QAction *RotationFilter::createAction(QObject *parent)
{
    QAction *action = new QAction(parent);
    action->setIcon(QIcon(":resources/icons/rotate-right.svg"));
    action->setText("&Rotation");
    action->setStatusTip("Rotation Filter");
    action->setToolTip(action->statusTip());

    return action;
}

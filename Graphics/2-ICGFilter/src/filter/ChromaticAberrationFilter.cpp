#include "ChromaticAberrationFilter.h"
#include <qglobal.h>

#include <QRgb>
#include <QTranslator>

ChromaticAberrationFilter::ChromaticAberrationFilter() : Filter("Chromatic Aberration Fitler")
{
    setParameter("intensity", 5);
}

void ChromaticAberrationFilter::apply(const QImage &input, QImage &output) const
{
    Q_ASSERT(input.rect() == output.rect());
    const QRgb *inputPixels = reinterpret_cast<const QRgb *>(input.bits());
    QRgb *outputPixels = reinterpret_cast<QRgb *>(output.bits());

    int intensity = parameters["intensity"].toInt();

    int width = input.width();
    int height = input.height();

    QPointF center(width / 2.0, height / 2.0);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double dx = x - center.x();
            double dy = y - center.y();

            double distance = std::sqrt(dx * dx + dy * dy);

            double nx = dx / center.x();
            double ny = dy / center.y();

            int redX = qBound(0, static_cast<int>(x + nx * intensity), width - 1);
            int redY = qBound(0, static_cast<int>(y + ny * intensity), height - 1);
            
            int blueX = qBound(0, static_cast<int>(x - nx * intensity), width - 1);
            int blueY = qBound(0, static_cast<int>(y - ny * intensity), height - 1);

            int r = qRed(inputPixels[redY * width + redX]);
            int g = qGreen(inputPixels[y * width + x]);
            int b = qBlue(inputPixels[blueY * width + blueX]);

            outputPixels[y * width + x] = qRgb(r, g, b);            
        }
    }
}

QList<Filter::ParameterInfo> ChromaticAberrationFilter::getParameterInfoList() const
{
    return {{"intensity", "Intensity", 5, 1, 100, "int", {}}};
}

void ChromaticAberrationFilter::setParameter(const QString& name, const QVariant& value)
{
    Filter::setParameter(name, value);
}

QAction *ChromaticAberrationFilter::createAction(QObject *parent)
{
    QAction *action = new QAction(parent);
    action->setIcon(QIcon(":resources/icons/sun-wireless.svg"));
    action->setText("&Chromatic Aberration");
    action->setStatusTip("Chromatic Aberration Filter");
    action->setToolTip(action->statusTip());

    return action;
}

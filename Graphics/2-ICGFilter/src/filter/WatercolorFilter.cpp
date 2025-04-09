#include "WatercolorFilter.h"
#include <qglobal.h>

#include <QRgb>
#include <QTranslator>
#include "SharpenFilter.h"

WatercolorFilter::WatercolorFilter() : Filter("Watercolor Filter")
{
    setParameter("intensity", 5);
    setParameter("radius", 2);
}

void WatercolorFilter::apply(const QImage &input, QImage &output) const
{
    Q_ASSERT(input.rect() == output.rect());
    QImage temp(output.size(), output.format());

    const QRgb *inputPixels = reinterpret_cast<const QRgb *>(input.bits());
    QRgb *tempPixels = reinterpret_cast<QRgb *>(temp.bits());

    int intensity = parameters["intensity"].toInt();
    int radius = parameters["radius"].toInt();

    int width = input.width();
    int height = input.height();

    QPointF center(width / 2.0, height / 2.0);

    #pragma omp parallel for
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            tempPixels[y * width + x] = medianOnce(inputPixels, width, height, y, x, radius); 
        }
    }

    SharpenFilter sharpen;
    sharpen.setParameter("intensity", parameters["intensity"]);
    sharpen.apply(temp, output);
}

QList<Filter::ParameterInfo> WatercolorFilter::getParameterInfoList() const
{
    return {{"intensity", "Intensity", 5, 1, 100, "int", {}},
            {"radius", "Radius", 2, 1, 30, "int", {}}};
}

void WatercolorFilter::setParameter(const QString& name, const QVariant& value)
{
    Filter::setParameter(name, value);

    if (name == "intensity") {
        int intensity = parameters["intensity"].toInt();
        kernel = sharpenKernel(intensity);
    }
}

QAction *WatercolorFilter::createAction(QObject *parent)
{
    QAction *action = new QAction(parent);
    action->setIcon(QIcon(":resources/icons/palette-outline.svg"));
    action->setText("&Watercolor");
    action->setStatusTip("Watercolor Filter");
    action->setToolTip(action->statusTip());

    return action;
}

std::vector<double> WatercolorFilter::sharpenKernel(int intensity)
{
    double i = static_cast<double>(intensity);
    double f = (intensity - 1) / 4.0;

    return {0, -f, 0,
            -f, i, -f,
            0, -f, 0};

}


#include "SharpenFilter.h"
#include <qglobal.h>

#include <QRgb>
#include <QTranslator>

SharpenFilter::SharpenFilter() : Filter("Sharpen Filter")
{
    setParameter("intensity", 5);
}

void SharpenFilter::apply(const QImage &input, QImage &output) const
{
    Q_ASSERT(input.rect() == output.rect());

    convolve(input, output, kernel, 3);
}

QList<Filter::ParameterInfo> SharpenFilter::getParameterInfoList() const
{
    return {{"intensity", "Intensity", 5, 1, 100, "int", {}}};
}

void SharpenFilter::setParameter(const QString& name, const QVariant& value)
{
    Filter::setParameter(name, value);

    if (name == "intensity") {
        int intensity = parameters["intensity"].toInt();
        kernel = sharpenKernel(intensity);
    }
}

QAction *SharpenFilter::createAction(QObject *parent)
{
    QAction *action = new QAction(parent);
    action->setIcon(QIcon(":resources/icons/sharpness.png"));
    action->setText("&Sharpness");
    action->setStatusTip("Sharpness Filter");
    action->setToolTip(action->statusTip());

    return action;
}

std::vector<double> SharpenFilter::sharpenKernel(int intensity)
{
    double i = static_cast<double>(intensity);
    double f = (intensity - 1) / 4.0;

    return {0, -f, 0,
            -f, i, -f,
            0, -f, 0};

}


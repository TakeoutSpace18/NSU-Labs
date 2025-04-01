#include "BWFilter.h"

#include <QRgb>
#include <QTranslator>

BWFilter::BWFilter() : Filter("Black/White Filter")
{
    parameters["intensity"] = 100;
}

void BWFilter::apply(const QImage &input, QImage &output) const
{
    Q_ASSERT(input.rect() == output.rect());

    const QRgb *inputPixels = reinterpret_cast<const QRgb *>(input.bits());
    QRgb *outputPixels = reinterpret_cast<QRgb *>(output.bits());

    float intensity = parameters["intensity"].toFloat() / 100.0;

#pragma omp parallel for
    for (size_t i = 0; i < input.width() * input.height(); ++i) {
        float r = qRed(inputPixels[i]);
        float g = qGreen(inputPixels[i]);
        float b = qBlue(inputPixels[i]);

        float y = 0.299 * r + 0.587 * g + 0.114 * b;
        outputPixels[i] = qRgb(y * intensity + r * (1 - intensity),
                               y * intensity + g * (1 - intensity),
                               y * intensity + b * (1 - intensity));
    }
}

QList<Filter::ParameterInfo> BWFilter::getParameterInfoList() const
{
    return {{"intensity", "Intensity", 100, 0, 100, "int", {}}};
}

QAction *BWFilter::createAction(QObject *parent)
{
    QAction *action = new QAction(parent);
    action->setIcon(QIcon(":resources/icons/image-filter-black-white.svg"));
    action->setText("&B/W");
    action->setStatusTip("B/W Filter");
    action->setToolTip(action->statusTip());

    return action;
}

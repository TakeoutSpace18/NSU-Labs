#include "InversionFilter.h"

#include <QRgb>
#include <QTranslator>

InversionFilter::InversionFilter() : Filter("Inversion filter")
{
}

void InversionFilter::apply(const QImage &input, QImage &output) const
{
    Q_ASSERT(input.rect() == output.rect());

    const QRgb *inputPixels = reinterpret_cast<const QRgb *>(input.bits());
    QRgb *outputPixels = reinterpret_cast<QRgb *>(output.bits());

#pragma omp parallel for
    for (size_t i = 0; i < input.width() * input.height(); ++i) {
        uint8_t r = qRed(inputPixels[i]);
        uint8_t g = qGreen(inputPixels[i]);
        uint8_t b = qBlue(inputPixels[i]);

        outputPixels[i] = qRgb(~r, ~g, ~b);
    }
}

QList<Filter::ParameterInfo> InversionFilter::getParameterInfoList() const
{
    return {};
}

QAction *InversionFilter::createAction(QObject *parent)
{
    QAction *action = new QAction(parent);
    action->setIcon(QIcon(":resources/icons/invert.png"));
    action->setText("&Inversion");
    action->setStatusTip("Inversion Filter");
    action->setToolTip(action->statusTip());

    return action;
}

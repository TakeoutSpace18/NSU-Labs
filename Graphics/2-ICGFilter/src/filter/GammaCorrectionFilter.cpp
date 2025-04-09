#include "GammaCorrectionFilter.h"

#include <QRgb>
#include <QTranslator>
#include <cmath>

GammaCorrectionFilter::GammaCorrectionFilter() : Filter("Gamma Correction Filter")
{
    setParameter("red", 1);
    setParameter("green", 1);
    setParameter("blue", 1);
    setParameter("gamma", 1);
}

void GammaCorrectionFilter::apply(const QImage &input, QImage &output) const
{
    Q_ASSERT(input.rect() == output.rect());

    const QRgb *inputPixels = reinterpret_cast<const QRgb *>(input.bits());
    QRgb *outputPixels = reinterpret_cast<QRgb *>(output.bits());
    double rgamma = parameters["red"].toDouble();
    double ggamma = parameters["green"].toDouble();
    double bgamma = parameters["blue"].toDouble();
    double gamma = parameters["gamma"].toDouble();

    auto gamma_correct = [](uint8_t x, double gamma) {
        return qBound(0,
                      static_cast<int>(pow(x / 255.0, gamma) * 255.0 + 0.5),
                      255);
    };

    #pragma omp parallel for
    for (size_t i = 0; i < input.width() * input.height(); ++i) {
        float r = qRed(inputPixels[i]);
        float g = qGreen(inputPixels[i]);
        float b = qBlue(inputPixels[i]);

        float y = 0.299 * r + 0.587 * g + 0.114 * b;
        outputPixels[i] = qRgb(gamma_correct(r, gamma),
                               gamma_correct(g, gamma),
                               gamma_correct(b, gamma));
    }
}

QList<Filter::ParameterInfo> GammaCorrectionFilter::getParameterInfoList() const
{
    return {
            // {"red", "Red", 1, 0.1, 10, "double", {}},
            // {"green", "Green", 1, 0.1, 10, "double", {}},
            // {"blue", "Blue", 1, 0.1, 10, "double", {}},
            {"gamma", "Gamma", 1, 0.1, 10, "double", {}}};
}

QAction *GammaCorrectionFilter::createAction(QObject *parent)
{
    QAction *action = new QAction(parent);
    action->setIcon(QIcon(":resources/icons/gamma.svg"));
    action->setText("&Gamma Correction");
    action->setStatusTip("Gamma Correction Filter");
    action->setToolTip(action->statusTip());

    return action;
}

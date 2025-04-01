#include "EmbossFilter.h"
#include <qglobal.h>

#include <QRgb>
#include <QTranslator>
#include <algorithm>

EmbossFilter::EmbossFilter() : Filter("Emboss Filter")
{
    setParameter("intensity", 5);
}

void EmbossFilter::apply(const QImage &input, QImage &output) const
{
    Q_ASSERT(input.rect() == output.rect());

    convolve(input, output, kernel, 3);

    const QRgb *inputPixels = reinterpret_cast<const QRgb *>(input.bits());
    QRgb *outputPixels = reinterpret_cast<QRgb *>(output.bits());

    int width = input.width();
    int height = input.height();

    std::transform(outputPixels, outputPixels + width * height, outputPixels, [](QRgb col) {
        return qRgb(
            qMin(qRed(col) + 128, 255),
            qMin(qGreen(col) + 128, 255),
            qMin(qBlue(col) + 128, 255)
        );
    });
}

QList<Filter::ParameterInfo> EmbossFilter::getParameterInfoList() const
{
    return {{"flip_horizontally", "Flip horozontally", false, 0, 0, "bool", {}},
            {"flip_vertically", "Flip vertically", false, 0, 0, "bool", {}}};
}

void EmbossFilter::setParameter(const QString& name, const QVariant& value)
{
    Filter::setParameter(name, value);

    bool flip_horizontally = parameters["flip_horizontally"].toBool();
    bool flip_vertically = parameters["flip_vertically"].toBool();
    kernel = embossKernel(flip_horizontally, flip_vertically);
}

QAction *EmbossFilter::createAction(QObject *parent)
{
    QAction *action = new QAction(parent);
    action->setIcon(QIcon(":resources/icons/church-outline.svg"));
    action->setText("&Emboss");
    action->setStatusTip("Emboss Filter");
    action->setToolTip(action->statusTip());

    return action;
}

std::vector<double> EmbossFilter::embossKernel(bool flip_horizontally,
                                               bool flip_vertically)
{
    std::vector<double> kernel = {0,  1,  0,
                                  1,  0, -1,
                                  0, -1,  0};
    if (flip_horizontally) {
        std::swap(kernel[3], kernel[5]);
    }
    if (flip_vertically) {
        std::swap(kernel[1], kernel[7]);
    }

    return kernel;
}


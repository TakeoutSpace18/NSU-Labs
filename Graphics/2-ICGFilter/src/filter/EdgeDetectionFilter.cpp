#include "EdgeDetectionFilter.h"

#include <QRgb>
#include <iostream>
#include <QTranslator>

EdgeDetectionFilter::EdgeDetectionFilter() : Filter("Edge Detection Filter")
{
    setParameter("threshold", 50);
    setParameter("use_threshold", false);
    setParameter("type", "Roberts");
}

void EdgeDetectionFilter::apply(const QImage &input, QImage &output) const
{
    if (parameters["type"].toString() == "Roberts"){
        applyRoberts(input, output);
    }
    else if (parameters["type"].toString() == "Sobel") {
        applySobel(input, output);
    }
    else {
        qDebug() << "Unknown EdgeDetectionFilter type parameter";
    }
}

void EdgeDetectionFilter::applyRoberts(const QImage& input, QImage& output) const
{
    Q_ASSERT(input.rect() == output.rect());
    const QRgb *inputPixels = reinterpret_cast<const QRgb *>(input.bits());
    QRgb *outputPixels = reinterpret_cast<QRgb *>(output.bits());

    int width = input.width();
    int height = input.height();

    bool use_threshold = parameters["use_threshold"].toBool();
    int threshold = parameters["threshold"].toInt() * 255 / 100;

    std::vector<double> kernel_x = {+1,  0,
                                     0, -1};

    std::vector<double> kernel_y = { 0, +1,
                                    -1,  0};

    #pragma omp parallel for
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            double gx = convolveOnceGray(inputPixels, width, height, i, j, kernel_x, 2);
            double gy = convolveOnceGray(inputPixels, width, height, i, j, kernel_y, 2);

            int result = qBound(0, static_cast<int>(qSqrt(gx * gx + gy * gy) + 0.5), 255);

            if (use_threshold) {
                result = result >= threshold ? 255 : 0;
            }

            outputPixels[i * width + j] = qRgb(result, result, result);
        }
    }
}

void EdgeDetectionFilter::applySobel(const QImage& input, QImage& output) const
{
    Q_ASSERT(input.rect() == output.rect());
    const QRgb *inputPixels = reinterpret_cast<const QRgb *>(input.bits());
    QRgb *outputPixels = reinterpret_cast<QRgb *>(output.bits());

    int width = input.width();
    int height = input.height();

    bool use_threshold = parameters["use_threshold"].toBool();
    int threshold = parameters["threshold"].toInt() * 255 / 100;

    std::vector<double> kernel_x = {+1,  0, -1
                                    +2,  0, -2,
                                    +1,  0, -1};

    std::vector<double> kernel_y = {+1, +2, +1,
                                     0,  0,  0,
                                    -1, -2, -1};

    #pragma omp parallel for
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            double gx = convolveOnceGray(inputPixels, width, height, i, j, kernel_x, 3);
            double gy = convolveOnceGray(inputPixels, width, height, i, j, kernel_y, 3);

            int result = qBound(0, static_cast<int>(qSqrt(gx * gx + gy * gy) + 0.5), 255);

            if (use_threshold) {
                result = result >= threshold ? 255 : 0;
            }

            outputPixels[i * width + j] = qRgb(result, result, result);
        }
    }
}

QList<Filter::ParameterInfo> EdgeDetectionFilter::getParameterInfoList() const
{
    return {{"type", "Type", "Roberts", 0, 0, "choice", {"Roberts", "Sobel"}},
            {"threshold", "Threshold", 50, 0, 100, "int", {}},
            {"use_threshold", "Use threshold", true, 0, 0, "bool", {}}};
}

void EdgeDetectionFilter::setParameter(const QString& name, const QVariant& value)
{
    Filter::setParameter(name, value);

    bool flip_horizontally = parameters["flip_horizontally"].toBool();
    bool flip_vertically = parameters["flip_vertically"].toBool();
    kernel = embossKernel(flip_horizontally, flip_vertically);
}

QAction *EdgeDetectionFilter::createAction(QObject *parent)
{
    QAction *action = new QAction(parent);
    action->setIcon(QIcon(":resources/icons/square-circle-outline.svg"));
    action->setText("&Edge Detection");
    action->setStatusTip("Edge Detection Filter");
    action->setToolTip(action->statusTip());

    return action;
}

std::vector<double> EdgeDetectionFilter::embossKernel(bool flip_horizontally,
                                               bool flip_vertically)
{
    std::vector<double> kernel = {0,  1,  0,
                                  1,  0, -1,
                                  0, -1,  0};
    return kernel;
}


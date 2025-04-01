#include "BlurFilter.h"

#include <QRgb>
#include <QTranslator>

BlurFilter::BlurFilter() : Filter("Gaussian Blur Filter")
{
    setParameter("radius", 1);
}

void BlurFilter::apply(const QImage &input, QImage &output) const
{
    Q_ASSERT(input.rect() == output.rect());
    int ksize = 2 * parameters["radius"].toInt() + 1;

    convolve(input, output, kernel, ksize);
}

QList<Filter::ParameterInfo> BlurFilter::getParameterInfoList() const
{
    return {{"radius", "Radius", 1, 1, 100, "int", {}}};
}

void BlurFilter::setParameter(const QString& name, const QVariant& value)
{
    Filter::setParameter(name, value);

    if (name == "radius") {
        int radius = parameters["radius"].toInt();
        int size = 2 * radius + 1;
        double sigma = 0.4 * radius + 0.6;
        kernel = gaussianKernel(sigma, size);
    }
}

QAction *BlurFilter::createAction(QObject *parent)
{
    QAction *action = new QAction(parent);
    action->setIcon(QIcon(":resources/icons/blur.svg"));
    action->setText("&Gaussian Blur");
    action->setStatusTip("Gaussian Blur Filter");
    action->setToolTip(action->statusTip());

    return action;
}

std::vector<double> BlurFilter::gaussianKernel(double sigma, int size)
{
    std::vector<double> kernel(size * size, 0);
    float sum = 0.0f;
    int half = size / 2;
    
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            int x = i - half;
            int y = j - half;
            kernel[i * size + j] = std::exp(-(x * x + y * y) / (2 * sigma * sigma));
            sum += kernel[i * size + j];
        }
    }
    
    // Normalize the kernel
    for (int i = 0; i < size * size; ++i) {
        kernel[i] /= sum;
    }

    return kernel;
}


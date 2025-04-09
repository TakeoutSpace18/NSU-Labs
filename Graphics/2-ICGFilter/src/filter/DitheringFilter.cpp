#include "DitheringFilter.h"
#include <qglobal.h>
#include <qimage.h>

#include <QRgb>
#include <QTranslator>
#include <QColorSpace>
#include <algorithm>
#include <cmath>

DitheringFilter::DitheringFilter() : Filter("Dithering Filter")
{
    setParameter("method", "Floyd-Steinberg");
    setParameter("quantization_red", 6);
    setParameter("quantization_green", 6);
    setParameter("quantization_blue", 6);
}

void DitheringFilter::apply(const QImage &input, QImage &output) const
{
    if (parameters["method"].toString() == "Floyd-Steinberg") {
        applyFloydSteinberg(input, output);
    }
    else if (parameters["method"].toString() == "Ordered") {
        applyOrdered(input, output);
    }
    else {
        qDebug() << "Unknown DitheringFilter method parameter";
    }
}

void DitheringFilter::applyFloydSteinberg(const QImage& input, QImage& output) const
{
    Q_ASSERT(input.rect() == output.rect());
    QImage inputCopy = input;
    QRgb *inputPixels = reinterpret_cast<QRgb *>(inputCopy.bits());
    QRgb *outputPixels = reinterpret_cast<QRgb *>(output.bits());

    int width = input.width();
    int height = input.height();

    int quantization_red = parameters["quantization_red"].toInt();
    int quantization_green = parameters["quantization_green"].toInt();
    int quantization_blue = parameters["quantization_blue"].toInt();

    #pragma omp parallel for
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            int oldR = qRed(inputPixels[i * width + j]);
            int oldG = qGreen(inputPixels[i * width + j]);
            int oldB = qBlue(inputPixels[i * width + j]);

            int newR = findClosestColor(oldR, quantization_red);
            int newG = findClosestColor(oldG, quantization_green);
            int newB = findClosestColor(oldB, quantization_blue);

            outputPixels[i * width + j] = qRgb(newR, newG, newB);

            int errR = oldR - newR;
            int errG = oldG - newG;
            int errB = oldB - newB;

            auto spread = [&](int dj, int di, float factor) {
                int ni = i + di;
                int nj = j + dj;
                if (nj >= 0 && nj < width && ni >= 0 && ni < height) {
                    QRgb c = inputPixels[ni * width + nj];
                    int r = qBound(0, qRed(c)   + qRound(errR * factor), 255);
                    int g = qBound(0, qGreen(c) + qRound(errG * factor), 255);
                    int b = qBound(0, qBlue(c)  + qRound(errB * factor), 255);
                    inputPixels[ni * width + nj] = qRgb(r, g, b);
                }
            };

            spread(+1,  0, 7.0f / 16);
            spread(-1, +1, 3.0f / 16);
            spread( 0, +1, 5.0f / 16);
            spread(+1, +1, 1.0f / 16);
        }
    }
}

void DitheringFilter::applyOrdered(const QImage& input, QImage& output) const
{
    Q_ASSERT(input.rect() == output.rect());
    const QRgb *inputPixels = reinterpret_cast<const QRgb *>(input.bits());
    QRgb *outputPixels = reinterpret_cast<QRgb *>(output.bits());

    int width = input.width();
    int height = input.height();

    int quantizationRed = parameters["quantization_red"].toInt();
    int quantizationGreen = parameters["quantization_green"].toInt();
    int quantizationBlue = parameters["quantization_blue"].toInt();

    int matrixSizeR = chooseMatrixSize(quantizationRed);
    int matrixSizeG = chooseMatrixSize(quantizationGreen);
    int matrixSizeB = chooseMatrixSize(quantizationBlue);

    std::vector<double> bayerR = generateBayerMatrix(matrixSizeR);
    std::vector<double> bayerG = generateBayerMatrix(matrixSizeG);
    std::vector<double> bayerB = generateBayerMatrix(matrixSizeB);

    qDebug() << matrixSizeR << matrixSizeB << matrixSizeG;

    #pragma omp parallel for
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            int r = qRed(inputPixels[i * width + j]);
            int g = qGreen(inputPixels[i * width + j]);
            int b = qBlue(inputPixels[i * width + j]);

            double thresholdR = bayerR[(i % matrixSizeR) * matrixSizeR + (j % matrixSizeR)];
            double thresholdG = bayerR[(i % matrixSizeG) * matrixSizeG + (j % matrixSizeG)];
            double thresholdB = bayerR[(i % matrixSizeB) * matrixSizeB + (j % matrixSizeB)];

            auto dither = [&](int value, int quantization, double threshold) -> int {
                double step = 255.0 / quantization;
                int shifted = qRound(qBound(0.0, value + step * threshold, 255.0));
                return findClosestColor(shifted, quantization);
            };

            r = dither(r, quantizationRed, thresholdR);
            g = dither(g, quantizationGreen, thresholdG);
            b = dither(b, quantizationBlue, thresholdB);

            outputPixels[i * width + j] = qRgb(r, g, b);
        }
    }
}

QList<Filter::ParameterInfo> DitheringFilter::getParameterInfoList() const
{
    return {
        {"method", "Method", "Floyd-Steinberg", 0, 0, "choice", {"Floyd-Steinberg", "Ordered"}},
        {"quantization_red", "Red Quantization", 6, 2, 128, "int", {}},
        {"quantization_green", "Green Quantization", 6, 2, 128, "int", {}},
        {"quantization_blue", "Blue Quantization", 6, 2, 128, "int", {}}
    };
}

void DitheringFilter::setParameter(const QString& name, const QVariant& value)
{
    Filter::setParameter(name, value);
}

QAction *DitheringFilter::createAction(QObject *parent)
{
    QAction *action = new QAction(parent);
    action->setIcon(QIcon(":resources/icons/gradient-horizontal.svg"));
    action->setText("&Dithering");
    action->setStatusTip("Dithering Filter");
    action->setToolTip(action->statusTip());

    return action;
}

int DitheringFilter::findClosestColor(int color, int quantization) const
{
    int step = 255 / (quantization - 1);
    return qRound(double(color) / 255 * (quantization - 1)) * step;
}

std::vector<double> DitheringFilter::generateBayerMatrix(int size) const
{
    if ((size & (size - 1)) != 0 || size < 2) {
        throw std::invalid_argument("Size must be a power of two and >= 2");
    }

    std::vector<double> matrix(2 * 2);

    matrix = {0.0 / 4, 2.0 / 4,
              3.0 / 4, 1.0 / 4};

    int currentSize = 2;
    while (currentSize < size) {
        int newSize = currentSize * 2;
        std::vector<double> newMatrix(newSize * newSize);

        double norm = 4 * currentSize * currentSize;

        for (int y = 0; y < currentSize; ++y) {
            for (int x = 0; x < currentSize; ++x) {
                double val = matrix[y * currentSize + x];

                newMatrix[y * newSize + x] = val;
                newMatrix[y * newSize + x + currentSize] = val + 2.0 / norm;
                newMatrix[(y + currentSize) * newSize + x] = val + 3.0 / norm;
                newMatrix[(y + currentSize) * newSize + x + currentSize] = val + 1.0 / norm;
            }
        }

        matrix = std::move(newMatrix);
        currentSize = newSize;
    }

    double max = *std::max_element(matrix.begin(), matrix.end());
    std::transform(matrix.begin(), matrix.end(), matrix.begin(), [&](double x) {
        return (x - 0.5 * max);
    });

    return matrix;
}

int DitheringFilter::chooseMatrixSize(int quantization) const
{
    auto roundUpToNextPowerOfTwo = [](int v) {
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v++;
        return v;
    };

    return roundUpToNextPowerOfTwo(qCeil(qSqrt(256.0 / quantization)));
}

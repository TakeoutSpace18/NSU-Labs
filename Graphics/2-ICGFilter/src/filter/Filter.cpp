#include "Filter.h"

void Filter::convolve(const QImage& input, QImage& output,
                      const std::vector<double>& kernel, int ksize) const
{
    Q_ASSERT(input.rect() == output.rect());

    const QRgb *inputPixels = reinterpret_cast<const QRgb *>(input.bits());
    QRgb *outputPixels = reinterpret_cast<QRgb *>(output.bits());

    int width = input.width();
    int height = input.height();

    int khalf = ksize / 2;

    #pragma omp parallel for
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {

            double rsum, gsum, bsum;
            rsum = gsum = bsum = 0;
            
            for (int ki = -khalf; ki < -khalf + ksize; ++ki) {
                for (int kj = -khalf; kj < -khalf + ksize; ++kj) {

                    int pi = qBound(0, i + ki, height - 1);
                    int pj = qBound(0, j + kj, width - 1);

                    float r = qRed(inputPixels[pi * width + pj]);
                    float g = qGreen(inputPixels[pi * width + pj]);
                    float b = qBlue(inputPixels[pi * width + pj]);

                    double weight = kernel[(ki + khalf) * ksize + (kj + khalf)];
                    rsum += r * weight;
                    gsum += g * weight;
                    bsum += b * weight;
                }
            }
            
            outputPixels[i * width + j] = qRgb(qBound(0.0, rsum, 255.0),
                                               qBound(0.0, gsum, 255.0),
                                               qBound(0.0, bsum, 255.0));
        }
    }
}

QRgb Filter::convolveOnce(const QRgb *inputPixels, int width, int height,
                          int i, int j,
                          const std::vector<double>& kernel, int ksize) const
{
    int khalf = ksize / 2;
    double rsum, gsum, bsum;
    rsum = gsum = bsum = 0;

    for (int ki = -khalf; ki < -khalf + ksize; ++ki) {
        for (int kj = -khalf; kj < -khalf + ksize; ++kj) {

            int pi = qBound(0, i + ki, height - 1);
            int pj = qBound(0, j + kj, width - 1);

            double r = qRed(inputPixels[pi * width + pj]);
            double g = qGreen(inputPixels[pi * width + pj]);
            double b = qBlue(inputPixels[pi * width + pj]);

            double weight = kernel[(ki + khalf) * ksize + (kj + khalf)];
            rsum += r * weight;
            gsum += g * weight;
            bsum += b * weight;
        }
    }

    return qRgb(rsum + 0.5, gsum + 0.5, bsum + 0.5);

}

double Filter::convolveOnceGray(const QRgb *inputPixels, int width, int height,
                                int i, int j,
                                const std::vector<double>& kernel, int ksize) const
{
    int khalf = ksize / 2;
    double ysum = 0;

    for (int ki = -khalf; ki < -khalf + ksize; ++ki) {
        for (int kj = -khalf; kj < -khalf + ksize; ++kj) {

            int pi = qBound(0, i + ki, height - 1);
            int pj = qBound(0, j + kj, width - 1);

            int y = qGray(inputPixels[pi * width + pj]);

            double weight = kernel[(ki + khalf) * ksize + (kj + khalf)];
            ysum += y * weight;
        }
    }

    return ysum;
}

#ifndef BLURFILTER_H
#define BLURFILTER_H

#include <qvariant.h>
#include "Filter.h"

class BlurFilter : public Filter
{
public:
    BlurFilter();

    void apply(const QImage& input, QImage& output) const override;

    QList<ParameterInfo> getParameterInfoList() const override;

    void setParameter(const QString& name, const QVariant& value) override;

    QAction* createAction(QObject* parent = nullptr) override;

private:
    std::vector<double> gaussianKernel(double sigma, int size);

    std::vector<double> kernel;
};

#endif  // !BLURFILTER_H

#ifndef EDGEDETECTIONFILTER_H
#define EDGEDETECTIONFILTER_H

#include <qvariant.h>
#include "Filter.h"

class EdgeDetectionFilter : public Filter
{
public:
    EdgeDetectionFilter();

    void apply(const QImage& input, QImage& output) const override;

    QList<ParameterInfo> getParameterInfoList() const override;

    void setParameter(const QString& name, const QVariant& value) override;

    QAction* createAction(QObject* parent = nullptr) override;

private:
    void applyRoberts(const QImage& input, QImage& output) const;
    void applySobel(const QImage& input, QImage& output) const;

    std::vector<double> embossKernel(bool flip_horizontally,
                                     bool flip_vertically);

    std::vector<double> kernel;
};

#endif  // !EDGEDETECTIONFILTER_H

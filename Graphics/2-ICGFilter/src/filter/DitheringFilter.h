#ifndef DITHERINGFILTER_H
#define DITHERINGFILTER_H

#include <QVariant>
#include "Filter.h"

class DitheringFilter : public Filter
{
public:
    DitheringFilter();

    void apply(const QImage& input, QImage& output) const override;
    void applyFloydSteinberg(const QImage& input, QImage& output) const;
    void applyOrdered(const QImage& input, QImage& output) const;

    QList<ParameterInfo> getParameterInfoList() const override;

    void setParameter(const QString& name, const QVariant& value) override;

    QAction* createAction(QObject* parent = nullptr) override;

private:
    int findClosestColor(int color, int levels) const;
    int chooseMatrixSize(int quantization) const;
    std::vector<double> generateBayerMatrix(int size) const;
};

#endif  // !DITHERINGFILTER_H

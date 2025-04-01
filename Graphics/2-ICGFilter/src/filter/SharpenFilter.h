#ifndef SHARPENFILLTER_H
#define SHARPENFILLTER_H

#include <qvariant.h>
#include "Filter.h"

class SharpenFilter : public Filter
{
public:
    SharpenFilter();

    void apply(const QImage& input, QImage& output) const override;

    QList<ParameterInfo> getParameterInfoList() const override;

    void setParameter(const QString& name, const QVariant& value) override;

    QAction* createAction(QObject* parent = nullptr) override;

private:
    std::vector<double> sharpenKernel(int intensity);

    std::vector<double> kernel;
};

#endif  // !SHARPENFILLTER_H

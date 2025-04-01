#ifndef EMBOSSFILTER_H
#define EMBOSSFILTER_H

#include <qvariant.h>
#include "Filter.h"

class EmbossFilter : public Filter
{
public:
    EmbossFilter();

    void apply(const QImage& input, QImage& output) const override;

    QList<ParameterInfo> getParameterInfoList() const override;

    void setParameter(const QString& name, const QVariant& value) override;

    QAction* createAction(QObject* parent = nullptr) override;

private:
    std::vector<double> embossKernel(bool flip_horizontally,
                                     bool flip_vertically);

    std::vector<double> kernel;
};

#endif  // !EMBOSSFILTER_H

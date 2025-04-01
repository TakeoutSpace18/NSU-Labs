#ifndef INVERSIONFILTER_H
#define INVERSIONFILTER_H

#include "Filter.h"

class InversionFilter : public Filter
{
public:
    InversionFilter();

    void apply(const QImage& input, QImage& output) const override;

    QList<ParameterInfo> getParameterInfoList() const override;

    QAction* createAction(QObject* parent = nullptr) override;
};

#endif  // !INVERSIONFILTER_H

#ifndef BWFILTER_H
#define BWFILTER_H

#include "Filter.h"

class BWFilter : public Filter
{
public:
    BWFilter();

    void apply(const QImage& input, QImage& output) const override;

    QList<ParameterInfo> getParameterInfoList() const override;

    QAction* createAction(QObject* parent = nullptr) override;
};

#endif  // !BWFILTER_H

#ifndef ROTATIONFILTER_H
#define ROTATIONFILTER_H

#include <QVariant>

#include "Filter.h"

class RotationFilter : public Filter
{
public:
    RotationFilter();

    void apply(const QImage& input, QImage& output) const override;

    QList<ParameterInfo> getParameterInfoList() const override;

    QAction* createAction(QObject* parent = nullptr) override;
};

#endif  // !ROTATIONFILTER_H

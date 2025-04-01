#ifndef GAMMACORRECTIONFILTER_H
#define GAMMACORRECTIONFILTER_H

#include "Filter.h"

class GammaCorrectionFilter : public Filter
{
public:
    GammaCorrectionFilter();

    void apply(const QImage& input, QImage& output) const override;

    QList<ParameterInfo> getParameterInfoList() const override;

    QAction* createAction(QObject* parent = nullptr) override;

private:
    std::array<uint8_t, 256> gammaLUT;
};

#endif  // !GAMMACORRECTIONFILTER_H

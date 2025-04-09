#ifndef CHROMATICABERRATIONFILTER_H
#define CHROMATICABERRATIONFILTER_H

#include <QVariant>
#include "Filter.h"

class ChromaticAberrationFilter : public Filter
{
public:
    ChromaticAberrationFilter();

    void apply(const QImage& input, QImage& output) const override;

    QList<ParameterInfo> getParameterInfoList() const override;

    void setParameter(const QString& name, const QVariant& value) override;

    QAction* createAction(QObject* parent = nullptr) override;

private:
};

#endif  // !CHROMATICABERRATIONFILTER_H

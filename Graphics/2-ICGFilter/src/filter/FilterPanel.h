#ifndef FILTERPANEL_H
#define FILTERPANEL_H

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMap>
#include <QPushButton>
#include <QSharedPointer>
#include <QSignalBlocker>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>

#include "Filter.h"

class FilterPanel : public QWidget
{
    Q_OBJECT

public:
    FilterPanel(QSharedPointer<Filter> filter, QWidget* parent = nullptr);
    ~FilterPanel();

    QSharedPointer<Filter> getFilter() const { return filter; };
    QString getFilterName() const { return filterName; };
    bool isAutoPreviewEnabled() const { return autoPreviewEnabled; };

public slots:
    void updateParameters(const QMap<QString, QVariant>& params);

signals:
    void applyFilter();
    void previewFilter();
    void panelClosed();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onParameterChanged();

private:
    void setupUI();
    QWidget* createParameterWidget(const Filter::ParameterInfo& paramInfo);

    QSharedPointer<Filter> filter;
    QString filterName;
    QMap<QString, QWidget*> parameterWidgets;

    bool autoPreviewEnabled = true;
};

#endif  // !FILTERPANEL_H

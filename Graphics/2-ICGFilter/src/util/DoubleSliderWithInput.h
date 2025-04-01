#ifndef DOUBLESLIDERWITHINPUT_H
#define DOUBLESLIDERWITHINPUT_H

#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QWidget>

class DoubleSliderWithInput : public QWidget
{
    Q_OBJECT

public:
    DoubleSliderWithInput(double minValue, double maxValue,
                          QWidget *parent = nullptr);

    explicit DoubleSliderWithInput(QWidget *parent = nullptr);

    double value() const { return m_spinBox->value(); }
    void setValue(double newValue);

    void setMaximum(double max);
    void setMinimum(double min);

signals:
    void valueChanged(double newValue);

private:
    QSlider *m_slider;
    QDoubleSpinBox *m_spinBox;

    double m_minValue;
    double m_maxValue;
};

#endif  // !DOUBLESLIDERWITHINPUT_H

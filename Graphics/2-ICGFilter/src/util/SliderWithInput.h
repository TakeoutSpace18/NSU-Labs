#ifndef SLIDER_WITH_INPUT_H
#define SLIDER_WITH_INPUT_H

#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QWidget>

class SliderWithInput : public QWidget
{
    Q_OBJECT

public:
    SliderWithInput(int minValue, int maxValue, QWidget *parent = nullptr);

    explicit SliderWithInput(QWidget *parent = nullptr);

    int value() const { return m_slider->value(); }
    void setValue(int newValue);

    void setMaximum(int max) { m_maxValue = max; }
    void setMinimum(int min) { m_minValue = min; }

signals:
    void valueChanged(int newValue);

private:
    QSlider *m_slider;
    QSpinBox *m_spinBox;

    int m_minValue;
    int m_maxValue;
};

#endif  // SLIDER_WITH_INPUT_H

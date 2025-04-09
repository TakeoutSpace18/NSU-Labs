#include "DoubleSliderWithInput.h"

#include <QIntValidator>
#include <QSpinBox>

DoubleSliderWithInput::DoubleSliderWithInput(QWidget *parent)
    : DoubleSliderWithInput(0, 100, parent) {};

DoubleSliderWithInput::DoubleSliderWithInput(double minValue, double maxValue, QWidget *parent)
    : QWidget(parent), m_minValue(minValue), m_maxValue(maxValue)
{
    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setRange(minValue * 10, maxValue * 10);

    m_spinBox = new QDoubleSpinBox(this);
    m_spinBox->setSingleStep(0.10);
    m_spinBox->setMinimum(minValue);
    m_spinBox->setMaximum(maxValue);

    connect(m_spinBox, &QSpinBox::editingFinished, this, [&]() {
        double value = m_spinBox->value();

        value = qBound(m_minValue, value, m_maxValue);
        setValue(value);
    });

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_slider);
    layout->addWidget(m_spinBox);
    setLayout(layout);

    connect(m_slider, &QSlider::valueChanged, this, [this](int value) {
        m_spinBox->setValue(value / 10.0);
        emit valueChanged(value / 10.0);
    });

    connect(m_spinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        value = qBound(m_minValue, value, m_maxValue);
        m_slider->setValue(value * 10);
        emit valueChanged(value);
    });
}

void DoubleSliderWithInput::setValue(double newValue)
{
    m_slider->setValue(newValue * 10);
    m_spinBox->setValue(newValue);
}

void DoubleSliderWithInput::setMaximum(double max)
{
    m_maxValue = max;
    m_slider->setRange(m_minValue * 10, m_maxValue * 10);
    m_spinBox->setRange(m_minValue, m_maxValue);
}
void DoubleSliderWithInput::setMinimum(double min)
{
    m_minValue = min;
    m_slider->setRange(m_minValue * 10, m_maxValue * 10);
    m_spinBox->setRange(m_minValue, m_maxValue);
}

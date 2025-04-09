#include "SliderWithInput.h"

#include <QIntValidator>
#include <QSpinBox>

SliderWithInput::SliderWithInput(QWidget *parent)
    : SliderWithInput(0, 100, parent) {};

SliderWithInput::SliderWithInput(int minValue, int maxValue, QWidget *parent)
    : QWidget(parent), m_minValue(minValue), m_maxValue(maxValue)
{
    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setRange(minValue, maxValue);

    m_spinBox = new QSpinBox(this);
    m_spinBox->setMinimum(minValue);
    m_spinBox->setMaximum(maxValue);

    connect(m_spinBox, &QSpinBox::editingFinished, this, [&]() {
        bool ok;
        int value = m_spinBox->text().toInt(&ok);
        if (!ok) {
            setValue(m_minValue);
            return;
        }

        value = qBound(m_minValue, value, m_maxValue);
        setValue(value);
    });

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_slider);
    layout->addWidget(m_spinBox);
    setLayout(layout);

    connect(m_slider, &QSlider::valueChanged, this, [this](int value) {
        m_spinBox->setValue(value);
        emit valueChanged(value);
    });

    connect(m_spinBox, &QSpinBox::valueChanged, this, [this](int value) {
        value = qBound(m_minValue, value, m_maxValue);
        m_slider->setValue(value);
        emit valueChanged(value);
    });
}

void SliderWithInput::setValue(int newValue)
{
    m_slider->setValue(newValue);
    m_spinBox->setValue(newValue);
}

void SliderWithInput::setMaximum(int max)
{
    m_maxValue = max;
    m_slider->setRange(m_minValue, m_maxValue);
    m_spinBox->setRange(m_minValue, m_maxValue);
}
void SliderWithInput::setMinimum(int min)
{
    m_minValue = min;
    m_slider->setRange(m_minValue, m_maxValue);
    m_spinBox->setRange(m_minValue, m_maxValue);
}

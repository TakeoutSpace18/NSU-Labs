#include "SliderWithInput.h"

#include <QIntValidator>

SliderWithInput::SliderWithInput(int minValue, int maxValue, QWidget *parent)
    : QWidget(parent), m_minValue(minValue), m_maxValue(maxValue) {

    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setRange(minValue, maxValue);

    m_inputField = new QLineEdit(this);
    m_inputField->setFixedWidth(35);

    connect(m_inputField, &QLineEdit::editingFinished, this, [&]() {
        bool ok;
        int value = m_inputField->text().toInt(&ok);
        if (!ok) {
            setValue(m_minValue);
            return;
        }

        if (value < m_minValue) {
            setValue(m_minValue);
        }
        else if (value > m_maxValue) {
            setValue(m_maxValue);
        }
    });

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_slider);
    layout->addWidget(m_inputField);
    setLayout(layout);

    connect(m_slider, &QSlider::valueChanged, this, [this](int value) {
        m_inputField->setText(QString::number(value));
        emit valueChanged(value);
    });

    connect(m_inputField, &QLineEdit::editingFinished, this, [this]() {
        int value = m_inputField->text().toInt();
        m_slider->setValue(value);
    });
}

int SliderWithInput::value() const {
    return m_slider->value();
}

void SliderWithInput::setValue(int newValue) {
    m_slider->setValue(newValue);
    m_inputField->setText(QString::number(newValue));
}


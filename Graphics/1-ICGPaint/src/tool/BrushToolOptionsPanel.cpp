#include "BrushToolOptionsPanel.h"

#include "ToolOptionsPanel.h"

#include <QSlider>
#include <QLineEdit>
#include <QBoxLayout>
#include <QLabel>

BrushToolOptionsPanel::BrushToolOptionsPanel(
    const QSharedPointer<BrushToolOptions>& options,
    QWidget *parent)
    : ToolOptionsPanel("Brush tool", parent),
    m_options(options)
{
    QLabel *widthLabel = new QLabel(this);
    widthLabel->setText("Width");
    m_slider = new SliderWithInput(1, 100, this);
    m_slider->setMinimumWidth(220);
    m_slider->setValue(options->width());

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(widthLabel);
    layout->addWidget(m_slider);
    layout->addStretch();

    setLayout(layout);

    connect(m_slider, &SliderWithInput::valueChanged, this, [this](int value) {
        if (isAutoApply()) {
            m_options->setWidth(value);
        }
    });

    connect(m_options.data(), &BrushToolOptions::widthChanged, m_slider, &SliderWithInput::setValue);
}

void BrushToolOptionsPanel::apply()
{
    m_options->setWidth(m_slider->value());
}


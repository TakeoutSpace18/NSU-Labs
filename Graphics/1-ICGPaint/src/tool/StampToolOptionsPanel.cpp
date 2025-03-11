#include "StampToolOptionsPanel.h"

#include "StampToolOptions.h"
#include "ToolOptionsPanel.h"

#include <QSlider>
#include <QLineEdit>
#include <QBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QConstOverload>

StampToolOptionsPanel::StampToolOptionsPanel(
    const QSharedPointer<StampToolOptions>& options,
    QWidget *parent)
    : ToolOptionsPanel("Stamp tool", parent),
    m_options(options)
{
    m_shapeLabel = new QLabel(this);
    m_shapeLabel->setText("Shape");
    m_shapeComboBox = new QComboBox(this);
    m_shapeComboBox->addItem("Polygon", QVariant::fromValue(StampToolOptions::Shape::Polygon));
    m_shapeComboBox->addItem("Star", QVariant::fromValue(StampToolOptions::Shape::Star));

    m_widthLabel = new QLabel(this);
    m_widthLabel->setText("Width");
    m_widthSlider = new SliderWithInput(1, 100, this);
    m_widthSlider->setMinimumWidth(220);
    m_widthSlider->setValue(options->width());

    m_radiusLabel = new QLabel(this);
    m_radiusLabel->setText("Radius");
    m_radiusSlider = new SliderWithInput(1, 1000, this);
    m_radiusSlider->setValue(options->radius());

    m_innerRadiusLabel = new QLabel(this);
    m_innerRadiusLabel->setText("Inner radius");
    m_innerRadiusSlider = new SliderWithInput(1, 1000, this);
    m_innerRadiusSlider->setValue(options->innerRadius());

    m_verticesLabel = new QLabel(this);
    m_verticesLabel->setText("Vertices");
    m_verticesSlider = new SliderWithInput(3, 100, this);
    m_verticesSlider->setValue(options->vertices());

    m_rotationLabel = new QLabel(this);
    m_rotationLabel->setText("Rotaion");
    m_rotationSlider = new SliderWithInput(1, 360, this);
    m_rotationSlider->setValue(options->rotation());

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_shapeLabel);
    layout->addWidget(m_shapeComboBox);
    layout->addWidget(m_widthLabel);
    layout->addWidget(m_widthSlider);
    layout->addWidget(m_radiusLabel);
    layout->addWidget(m_radiusSlider);
    layout->addWidget(m_innerRadiusLabel);
    layout->addWidget(m_innerRadiusSlider);
    layout->addWidget(m_verticesLabel);
    layout->addWidget(m_verticesSlider);
    layout->addWidget(m_rotationLabel);
    layout->addWidget(m_rotationSlider);
    layout->addStretch();

    setLayout(layout);

    connect(m_shapeComboBox, &QComboBox::currentIndexChanged, this, [this](int index) {
        if (isAutoApply()) {
            m_options->setShape(m_shapeComboBox->currentData().value<StampToolOptions::Shape>());
        }
    });
    connect(m_widthSlider, &SliderWithInput::valueChanged, this, [this](int value) {
        if (isAutoApply()) {
            m_options->setWidth(value);
        }
    });
    connect(m_radiusSlider, &SliderWithInput::valueChanged, this, [this](int value) {
        if (isAutoApply()) {
            m_options->setRadius(value);
        }
    });
    connect(m_innerRadiusSlider, &SliderWithInput::valueChanged, this, [this](int value) {
        if (isAutoApply()) {
            m_options->setInnerRadius(value);
        }
    });
    connect(m_verticesSlider, &SliderWithInput::valueChanged, this, [this](int value) {
        if (isAutoApply()) {
            m_options->setVertices(value);
        }
    });
    connect(m_rotationSlider, &SliderWithInput::valueChanged, this, [this](int value) {
        if (isAutoApply()) {
            m_options->setRotation(value);
        }
    });


    connect(m_options.data(), &StampToolOptions::shapeChanged, this, [&](StampToolOptions::Shape shape) {
        int index = m_shapeComboBox->findData(QVariant::fromValue(shape));
        m_shapeComboBox->setCurrentIndex(index);
        if (shape == StampToolOptions::Shape::Star) {
            m_innerRadiusSlider->show();
            m_innerRadiusLabel->show();
        }
        else {
            m_innerRadiusSlider->hide();
            m_innerRadiusLabel->hide();
        }
    });
    connect(m_options.data(), &StampToolOptions::widthChanged, m_widthSlider, &SliderWithInput::setValue);
    connect(m_options.data(), &StampToolOptions::radiusChanged, m_radiusSlider, &SliderWithInput::setValue);
    connect(m_options.data(), &StampToolOptions::innerRadiusChanged, m_innerRadiusSlider, &SliderWithInput::setValue);
    connect(m_options.data(), &StampToolOptions::verticesChanged, m_verticesSlider, &SliderWithInput::setValue);
    connect(m_options.data(), &StampToolOptions::rotationChanged, m_rotationSlider, &SliderWithInput::setValue);
}

void StampToolOptionsPanel::apply()
{
    m_options->setShape(m_shapeComboBox->currentData().value<StampToolOptions::Shape>());
    m_options->setWidth(m_widthSlider->value());
    m_options->setRadius(m_radiusSlider->value());
    m_options->setInnerRadius(m_innerRadiusSlider->value());
    m_options->setVertices(m_verticesSlider->value());
    m_options->setRotation(m_rotationSlider->value());
}


#include "FilterPanel.h"

#include <qcheckbox.h>
#include <qnamespace.h>

#include <QCloseEvent>

#include "util/SliderWithInput.h"

FilterPanel::FilterPanel(QSharedPointer<Filter> filter, QWidget *parent)
    : QWidget(parent), filter(filter), filterName(filter->getDisplayName())
{
    setupUI();
}

FilterPanel::~FilterPanel()
{
}

void FilterPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QCheckBox *autoPreviewCheckBox = new QCheckBox("&Auto preview", this);
    if (autoPreviewEnabled) {
        autoPreviewCheckBox->setCheckState(Qt::CheckState::Checked);
    }
    mainLayout->addWidget(autoPreviewCheckBox);

    connect(autoPreviewCheckBox, &QCheckBox::toggled, this,
            [this](bool checked) {
                autoPreviewEnabled = checked;
                if (autoPreviewEnabled) {
                    emit previewFilter();
                }
            });

    QGroupBox *paramGroup = new QGroupBox(tr("Settings"));
    QGridLayout *paramLayout = new QGridLayout(paramGroup);

    QList<Filter::ParameterInfo> filterParamInfos =
        filter->getParameterInfoList();

    if (!filterParamInfos.isEmpty()) {
        // Create widgets for each parameter
        int row = 0;
        for (const auto &paramInfo : filterParamInfos) {
            QLabel *label = new QLabel(paramInfo.displayName);
            QWidget *widget = createParameterWidget(paramInfo);

            parameterWidgets[paramInfo.name] = widget;

            paramLayout->addWidget(label, row, 0);
            paramLayout->addWidget(widget, row, 1);
            row++;
        }

        mainLayout->addWidget(paramGroup);
    }

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QPushButton *previewButton = new QPushButton(tr("&Preview"));
    QPushButton *applyButton = new QPushButton(tr("&Apply"));
    QPushButton *cancelButton = new QPushButton(tr("&Cancel"));

    connect(previewButton, &QPushButton::clicked, this,
            &FilterPanel::previewFilter);
    connect(applyButton, &QPushButton::clicked, this,
            &FilterPanel::applyFilter);
    connect(cancelButton, &QPushButton::clicked, this,
            [this]() { emit panelClosed(); });

    buttonLayout->addWidget(previewButton);
    buttonLayout->addWidget(applyButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();
    setLayout(mainLayout);
}

QWidget *FilterPanel::createParameterWidget(
    const Filter::ParameterInfo &paramInfo)
{
    QWidget *widget = nullptr;

    if (paramInfo.type == "int") {
        SliderWithInput *slider = new SliderWithInput();
        slider->setMinimum(paramInfo.minValue.toInt());
        slider->setMaximum(paramInfo.maxValue.toInt());
        slider->setValue(filter->getParameter(paramInfo.name).toInt());

        connect(slider, QOverload<int>::of(&SliderWithInput::valueChanged),
                this, &FilterPanel::onParameterChanged);

        widget = slider;
    } else if (paramInfo.type == "double") {
        QDoubleSpinBox *spinBox = new QDoubleSpinBox();
        spinBox->setMinimum(paramInfo.minValue.toDouble());
        spinBox->setMaximum(paramInfo.maxValue.toDouble());
        spinBox->setValue(filter->getParameter(paramInfo.name).toDouble());

        connect(spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, &FilterPanel::onParameterChanged);
        widget = spinBox;
    } else if (paramInfo.type == "bool") {
        QCheckBox *checkBox = new QCheckBox();
        checkBox->setChecked(filter->getParameter(paramInfo.name).toBool());

        connect(checkBox, &QCheckBox::toggled, this,
                &FilterPanel::onParameterChanged);
        widget = checkBox;
    } else if (paramInfo.type == "choice") {
        QComboBox *comboBox = new QComboBox();
        comboBox->addItems(paramInfo.choices);
        comboBox->setCurrentText(
            filter->getParameter(paramInfo.name).toString());

        connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &FilterPanel::onParameterChanged);
        widget = comboBox;
    }

    return widget;
}

void FilterPanel::onParameterChanged()
{
    QObject *senderWidget = sender();

    QString paramName;
    for (auto it = parameterWidgets.begin(); it != parameterWidgets.end();
         ++it) {
        if (it.value() == senderWidget) {
            paramName = it.key();
            break;
        }
    }

    if (paramName.isEmpty()) {
        return;
    }

    QVariant value;
    if (SliderWithInput *slider =
            qobject_cast<SliderWithInput *>(senderWidget)) {
        value = slider->value();
    } else if (QDoubleSpinBox *doubleSpinBox =
                   qobject_cast<QDoubleSpinBox *>(senderWidget)) {
        value = doubleSpinBox->value();
    } else if (QCheckBox *checkBox = qobject_cast<QCheckBox *>(senderWidget)) {
        value = checkBox->isChecked();
    } else if (QComboBox *comboBox = qobject_cast<QComboBox *>(senderWidget)) {
        value = comboBox->currentText();
    }

    filter->setParameter(paramName, value);

    if (autoPreviewEnabled) {
        emit previewFilter();
    }
}

void FilterPanel::updateParameters(const QMap<QString, QVariant> &params)
{
    filter->setAllParameters(params);

    // Update UI widgets
    for (auto it = parameterWidgets.begin(); it != parameterWidgets.end();
         ++it) {
        QString paramName = it.key();
        QWidget *widget = it.value();

        QVariant value = params.value(paramName);

        if (QSpinBox *spinBox = qobject_cast<QSpinBox *>(widget)) {
            spinBox->setValue(value.toInt());
        } else if (QDoubleSpinBox *doubleSpinBox =
                       qobject_cast<QDoubleSpinBox *>(widget)) {
            doubleSpinBox->setValue(value.toDouble());
        } else if (QCheckBox *checkBox = qobject_cast<QCheckBox *>(widget)) {
            checkBox->setChecked(value.toBool());
        } else if (QComboBox *comboBox = qobject_cast<QComboBox *>(widget)) {
            comboBox->setCurrentText(value.toString());
        }
    }
}

void FilterPanel::closeEvent(QCloseEvent *event)
{
    emit panelClosed();
    event->accept();
}

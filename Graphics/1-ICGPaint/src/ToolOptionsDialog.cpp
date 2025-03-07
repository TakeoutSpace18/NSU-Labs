#include "ToolOptionsDialog.h"
#include "ICGPaint.h"
#include "tool/BrushToolOptionsPanel.h"
#include "tool/LineToolOptionsPanel.h"
#include <qlabel.h>
#include <qnamespace.h>

ToolOptionsDialog::ToolOptionsDialog(ICGPaint::ToolOptions& toolOptions,
                                     QWidget *parent)
    : QDialog(parent), m_toolOptions(toolOptions) {

    setWindowTitle("Tool Options");

    m_toolOptionsPanels.append(new LineToolOptionsPanel(m_toolOptions.line, this));
    m_toolOptionsPanels.append(new BrushToolOptionsPanel(m_toolOptions.brush, this));

    m_okButton = new QPushButton("OK");
    m_cancelButton = new QPushButton("Cancel");

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    for (auto panel : m_toolOptionsPanels) {
        QLabel *nameLabel = new QLabel(panel->name(), this);
        nameLabel->setStyleSheet("font-weight: bold");
        mainLayout->addWidget(nameLabel);
        mainLayout->addWidget(panel);
    }
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    connect(m_okButton, &QPushButton::clicked, this, &ToolOptionsDialog::acceptSettings);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void ToolOptionsDialog::acceptSettings() {
    for (auto optionsPanel : m_toolOptionsPanels) {
        optionsPanel->apply();
    }

    accept();
}


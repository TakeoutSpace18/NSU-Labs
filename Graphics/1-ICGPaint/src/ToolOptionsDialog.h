#ifndef TOOL_OPTIONS_DIALOG_H
#define TOOL_OPTIONS_DIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>

#include "ICGPaint.h"
#include "tool/ToolOptionsPanel.h"

class ToolOptionsDialog : public QDialog {
    Q_OBJECT

public:
    ToolOptionsDialog(ICGPaint::ToolOptions& toolOptions,
                      QWidget *parent = nullptr);

private slots:
    void acceptSettings();

private:
    QList<ToolOptionsPanel*> m_toolOptionsPanels;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;

    ICGPaint::ToolOptions m_toolOptions;
};

#endif // TOOL_OPTIONS_DIALOG_H

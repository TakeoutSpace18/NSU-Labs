#ifndef TOOL_OPTIONS_PANEL_H
#define TOOL_OPTIONS_PANEL_H

#include <QWidget>
#include <QLabel>

class ToolOptionsPanel : public QFrame
{
    Q_OBJECT

public:
    ToolOptionsPanel(const QString& toolName, QWidget *parent = nullptr)
    : QFrame(parent), m_toolName(toolName), m_autoApply(false)
    {
        setFrameShape(QFrame::StyledPanel);
        setFrameShadow(QFrame::Raised);
        setLineWidth(5);
    }

    const QString& name() const {
        return m_toolName;
    }

    void setAutoApply(bool autoApply) {
        m_autoApply = autoApply;
    }

    bool isAutoApply() {
        return m_autoApply;
    }
    
    virtual void apply() = 0;
    virtual ~ToolOptionsPanel() = default;

private:
    QString m_toolName;
    bool m_autoApply;
};

#endif // !TOOL_OPTIONS_PANEL_H

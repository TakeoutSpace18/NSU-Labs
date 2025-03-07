#ifndef LINE_TOOL_OPTIONS_PANEL_H
#define LINE_TOOL_OPTIONS_PANEL_H

#include <QSlider>
#include <QLineEdit>

#include "LineTool.h"
#include "ToolOptionsPanel.h"
#include "util/SliderWithInput.h"

class LineToolOptionsPanel : public ToolOptionsPanel
{
    Q_OBJECT;

public:
    LineToolOptionsPanel(const QSharedPointer<LineToolOptions>& options,
                         QWidget *parent = nullptr);

    void setOptions(const QSharedPointer<LineToolOptions>& options) {
        m_options = options;
    }

    void apply() override;

private:
    QSharedPointer<LineToolOptions> m_options;

    SliderWithInput *m_slider;
};


#endif // !LINE_TOOL_OPTIONS_PANEL_H

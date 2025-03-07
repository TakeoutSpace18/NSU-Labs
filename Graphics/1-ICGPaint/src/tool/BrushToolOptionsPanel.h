#ifndef BRUSH_TOOL_OPTIONS_PANEL_H
#define BRUSH_TOOL_OPTIONS_PANEL_H

#include <QSlider>
#include <QLineEdit>

#include "ToolOptionsPanel.h"
#include "tool/BrushToolOptions.h"
#include "util/SliderWithInput.h"

class BrushToolOptionsPanel : public ToolOptionsPanel
{
    Q_OBJECT;

public:
    BrushToolOptionsPanel(const QSharedPointer<BrushToolOptions>& options,
                         QWidget *parent = nullptr);

    void setOptions(const QSharedPointer<BrushToolOptions>& options) {
        m_options = options;
    }

    void apply() override;

private:
    QSharedPointer<BrushToolOptions> m_options;

    SliderWithInput *m_slider;
};


#endif // !BRUSH_TOOL_OPTIONS_PANEL_H

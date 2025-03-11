#ifndef STAMP_TOOL_OPTIONS_PANEL_H
#define STAMP_TOOL_OPTIONS_PANEL_H

#include <QSlider>
#include <qcombobox.h>

#include "StampToolOptions.h"
#include "ToolOptionsPanel.h"
#include "util/SliderWithInput.h"

class StampToolOptionsPanel : public ToolOptionsPanel
{
    Q_OBJECT;

public:
    StampToolOptionsPanel(const QSharedPointer<StampToolOptions>& options,
                         QWidget *parent = nullptr);

    void setOptions(const QSharedPointer<StampToolOptions>& options) {
        m_options = options;
    }

    void apply() override;

private:
    QSharedPointer<StampToolOptions> m_options;

    QComboBox *m_shapeComboBox;
    SliderWithInput *m_widthSlider;
    SliderWithInput *m_radiusSlider;
    SliderWithInput *m_innerRadiusSlider;
    SliderWithInput *m_rotationSlider;
    SliderWithInput *m_verticesSlider;

    QLabel *m_shapeLabel;
    QLabel *m_widthLabel;
    QLabel *m_radiusLabel;
    QLabel *m_innerRadiusLabel;
    QLabel *m_rotationLabel;
    QLabel *m_verticesLabel;
};


#endif // !STAMP_TOOL_OPTIONS_PANEL_H

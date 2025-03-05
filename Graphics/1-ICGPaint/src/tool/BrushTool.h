#ifndef BRUSH_TOOL_H
#define BRUSH_TOOL_H

#include "Tool.h"
#include "tool/BrushToolOptions.h"

class BrushTool : public Tool
{
    Q_OBJECT;

public:
    static QIcon Icon();

    BrushTool(Canvas *canvas, const QSharedPointer<BrushToolOptions>& options,
              QWidget *parent = nullptr);

    void setOptions(const QSharedPointer<BrushToolOptions>& options) {
        m_options = options;
    }

    ToolOptionsPanel* createOptionsPanel(QWidget* parent = nullptr) const override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QPoint m_lastPoint;
    bool m_isPainting;

    QSharedPointer<BrushToolOptions> m_options;
};


#endif // !BRUSH_TOOL_H

#ifndef BRUSH_TOOL_H
#define BRUSH_TOOL_H

#include "Tool.h"

class BrushTool : public Tool
{
    Q_OBJECT;

public:
    struct Options
    {
        Options() : color(Qt::black), width(5) {}

        QColor color;
        int width;
    };

    static QIcon Icon();

    BrushTool(Canvas *canvas, const QSharedPointer<const Options>& options);

    void setOptions(const QSharedPointer<const Options>& options) {
        m_options = options;
    }

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QPoint m_lastPoint;
    bool m_isPainting;

    QSharedPointer<const BrushTool::Options> m_options;
};


#endif // !BRUSH_TOOL_H

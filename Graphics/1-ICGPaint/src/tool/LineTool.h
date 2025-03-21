#ifndef LINE_TOOL_H
#define LINE_TOOL_H

#include <QWidget>

#include "Tool.h"
#include "LineToolOptions.h"
#include "ToolOptionsPanel.h"

class LineTool : public Tool
{
    Q_OBJECT;

public:
    static QIcon Icon();

    LineTool(Canvas *canvas, const QSharedPointer<LineToolOptions>& options,
             QWidget *parent = nullptr);

    void setOptions(const QSharedPointer<LineToolOptions>& options) {
        m_options = options;
    }

    ToolOptionsPanel* createOptionsPanel(QWidget* parent = nullptr) const override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void drawPreview(QPoint a, QPoint b);

    QPoint m_firstPoint;
    bool m_firstPointSelected;

    QSharedPointer<LineToolOptions> m_options;
};


#endif // !LINE_TOOL_H

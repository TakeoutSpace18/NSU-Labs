#ifndef TOOL_H
#define TOOL_H

#include <QWidget>

#include "ToolOptionsPanel.h"
#include "Canvas.h"

class Tool : public QWidget
{
    Q_OBJECT

public:
    Tool(Canvas *canvas, QWidget *parent = nullptr);

    virtual ToolOptionsPanel* createOptionsPanel(QWidget* parent = nullptr) const {
        return nullptr;
    }

    void setCanvas(Canvas *canvas) {
        m_canvas = canvas;
    }

    virtual ~Tool() = default;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

    void clearPreview();

    Canvas *m_canvas;
    QImage m_preview;
};

#endif // !TOOL_H

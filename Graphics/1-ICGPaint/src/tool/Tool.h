#ifndef TOOL_H
#define TOOL_H

#include <QWidget>

#include "ToolOptionsPanel.h"
#include "Canvas.h"

class Tool : public QWidget
{
    Q_OBJECT

public:
    Tool(Canvas *canvas) : QWidget(canvas), m_canvas(canvas) {
        setGeometry(0, 0, canvas->width(), canvas->height());

        // Make the widget transparent
        setAttribute(Qt::WA_TransparentForMouseEvents, false);
        setAttribute(Qt::WA_NoSystemBackground, true);
        setAttribute(Qt::WA_TranslucentBackground, true);
        
        // Ensure this widget stays on top
        setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    }

    virtual ToolOptionsPanel* createOptionsPanel(QWidget* parent = nullptr) const {
        return nullptr;
    }


    virtual ~Tool() = default;

protected:
    Canvas *m_canvas;
};

#endif // !TOOL_H

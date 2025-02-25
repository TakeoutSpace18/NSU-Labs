#ifndef TOOL_H
#define TOOL_H

#include <QWidget>

#include "Canvas.h"

class Tool : public QWidget {
    Q_OBJECT

public:
    Tool(Canvas *canvas) : QWidget(canvas), m_canvas(canvas) {
        setGeometry(0, 0, canvas->width(), canvas->height());
    }

    virtual ~Tool() = default;

protected:
    Canvas *m_canvas;
};

#endif // !TOOL_H

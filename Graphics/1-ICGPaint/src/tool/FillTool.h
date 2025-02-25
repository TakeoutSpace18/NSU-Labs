#ifndef FILL_TOOL_H
#define FILL_TOOL_H

#include "Tool.h"

class FillTool : public Tool
{
    Q_OBJECT;

public:
    struct Options
    {
        Options() : color(Qt::black) {}

        QColor color;
    };

    static QIcon Icon();

    FillTool(Canvas *canvas, const QSharedPointer<const Options>& options);

    void setOptions(const QSharedPointer<const Options>& options) {
        m_options = options;
    }

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    void doSpanFilling(QPoint seed);

    QSharedPointer<const FillTool::Options> m_options;
};


#endif // !FILL_TOOL_H

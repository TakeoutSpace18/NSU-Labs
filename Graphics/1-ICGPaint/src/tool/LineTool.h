#ifndef LINE_TOOL_H
#define LINE_TOOL_H

#include "Tool.h"

class LineTool : public Tool
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

    LineTool(Canvas *canvas, const QSharedPointer<const Options>& options);

    void setOptions(const QSharedPointer<const Options>& options) {
        m_options = options;
    }

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void drawPreview(QPoint a, QPoint b);
    void resetPreview();

    QPoint m_firstPoint;
    bool m_firstPointSelected;

    QImage m_preview;

    QSharedPointer<const LineTool::Options> m_options;
};


#endif // !LINE_TOOL_H

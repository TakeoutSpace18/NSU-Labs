#ifndef FILL_TOOL_H
#define FILL_TOOL_H

#include "Tool.h"
#include "tool/FillToolOptions.h"

class FillTool : public Tool
{
    Q_OBJECT;

public:
    static QIcon Icon();

    FillTool(Canvas *canvas, const QSharedPointer<FillToolOptions>& options);

    void setOptions(const QSharedPointer<FillToolOptions>& options) {
        m_options = options;
    }

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    void doSpanFilling(QImage& image, QPoint seed, const QColor& color);

    QSharedPointer<FillToolOptions> m_options;
};


#endif // !FILL_TOOL_H

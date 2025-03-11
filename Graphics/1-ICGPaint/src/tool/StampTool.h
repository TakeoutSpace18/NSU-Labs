#ifndef STAMP_TOOL_H
#define STAMP_TOOL_H

#include <QWidget>
#include <qpoint.h>

#include "Tool.h"
#include "StampToolOptions.h"
#include "ToolOptionsPanel.h"

class StampTool : public Tool
{
    Q_OBJECT;

public:
    static QIcon Icon();
    static QIcon Icon(const StampToolOptions& options);

    StampTool(Canvas *canvas, const QSharedPointer<StampToolOptions>& options,
             QWidget *parent = nullptr);

    void setOptions(const QSharedPointer<StampToolOptions>& options) {
        m_options = options;
    }

    ToolOptionsPanel* createOptionsPanel(QWidget* parent = nullptr) const override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    static void Draw(QImage& image, QPoint center, const StampToolOptions& options);

    static QVector<QPoint> GeneratePolygonPoints(QPoint center, const StampToolOptions& options);
    static QVector<QPoint> GenerateStarPoints(QPoint center, const StampToolOptions& options);

    void drawPreview(QPoint center);

    QSharedPointer<StampToolOptions> m_options;
};


#endif // !STAMP_TOOL_H

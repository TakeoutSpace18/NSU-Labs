#ifndef ICGPAINT_H
#define ICGPAINT_H

#include <QMainWindow>
#include <QScrollArea>
#include <QSizeGrip>
#include <QColor>
#include <QDockWidget>

#include "Canvas.h"
#include "tool/Tool.h"

#include "tool/BrushToolOptions.h"
#include "tool/LineToolOptions.h"
#include "tool/FillToolOptions.h"

#include "tool/BrushTool.h"
#include "tool/LineTool.h"
#include "tool/FillTool.h"
#include "tool/ToolOptionsPanel.h"

class ICGPaint : public QMainWindow
{
    Q_OBJECT

public:
    ICGPaint();

    struct ToolOptions
    {
        ToolOptions();

        QSharedPointer<BrushToolOptions> brush;
        QSharedPointer<LineToolOptions> line;
        QSharedPointer<FillToolOptions> fill;
    };

private:
    void createActions();
    void createToolOptionsDock();
    QActionGroup *createColorActions();

    QIcon generateColorIcon(QColor color);
    void setActiveColor(QColor color);
    void setActiveTool(Tool *tool);

    void newFile();
    void clearCanvas();
    void toolOptions();
    void selectColorDialog();
    void aboutDialog();

    Canvas *m_canvas;
    QSizeGrip *m_sizeGrip;
    QScrollArea *m_scrollArea;

    QColor m_activeColor;
    Tool *m_activeTool;

    ToolOptions m_toolOptions;

    BrushTool *m_brushTool;
    LineTool *m_lineTool;
    FillTool *m_fillTool;

    QAction *m_newAction;
    QAction *m_clearAction;
    QAction *m_selectColorAction;
    QAction *m_brushToolAction;
    QAction *m_lineToolAction;
    QAction *m_fillToolAction;
    QAction *m_toolOptionsAction;
    QActionGroup *m_colorActions;

    QDockWidget *m_toolOptionsDock;
};
#endif // ICGPAINT_H

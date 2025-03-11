#ifndef ICGPAINT_H
#define ICGPAINT_H

#include <QMainWindow>
#include <QScrollArea>
#include <QSizeGrip>
#include <QColor>
#include <QDockWidget>
#include <QStackedLayout>

#include "Canvas.h"
#include "tool/Tool.h"

#include "tool/BrushToolOptions.h"
#include "tool/LineToolOptions.h"
#include "tool/FillToolOptions.h"
#include "tool/StampToolOptions.h"

#include "tool/BrushTool.h"
#include "tool/LineTool.h"
#include "tool/FillTool.h"
#include "tool/StampTool.h"

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
        QSharedPointer<StampToolOptions> stamp;
    };

private:
    void createActions();
    void createTools(Canvas *canvas);
    void createToolOptionsDock();
    QActionGroup *createColorActions();

    QIcon generateColorIcon(QColor color);
    void setActiveColor(QColor color);
    void setActiveTool(Tool *tool);

    void newFileDialog();
    void saveFile();
    void openFile();

    void setCanvas(Canvas *canvas);

    void clearCanvas();
    void toolOptions();
    void selectColorDialog();

    Canvas *m_canvas;
    QSizeGrip *m_sizeGrip;
    QScrollArea *m_scrollArea;

    QColor m_activeColor;

    ToolOptions m_toolOptions;

    QList<Tool*> m_tools;
    Tool *m_activeTool;

    BrushTool *m_brushTool;
    LineTool *m_lineTool;
    FillTool *m_fillTool;
    StampTool *m_stampTool;

    QStackedLayout *m_toolsLayout;

    QDockWidget *m_toolOptionsDock;

    QAction *m_newAction;
    QAction *m_openAction;
    QAction *m_saveAction;
    QAction *m_clearAction;
    QAction *m_selectColorAction;

    QAction *m_brushToolAction;
    QAction *m_lineToolAction;
    QAction *m_fillToolAction;
    QAction *m_stampToolAction;

    QAction *m_toolOptionsAction;
    QActionGroup *m_colorActions;

    QString m_savePath;
};
#endif // ICGPAINT_H

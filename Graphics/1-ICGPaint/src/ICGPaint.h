#ifndef ICGPAINT_H
#define ICGPAINT_H

#include <QMainWindow>
#include <QScrollArea>
#include <QSizeGrip>
#include <QColor>

#include "Canvas.h"
#include "tool/Tool.h"
#include "tool/BrushTool.h"
#include "tool/LineTool.h"
#include "tool/FillTool.h"

class ICGPaint : public QMainWindow
{
    Q_OBJECT

public:
    ICGPaint();

protected:

private:
    void createActions();
    QActionGroup *createColorActions();

    QIcon generateColorIcon(QColor color);
    void setActiveColor(QColor color);
    void setActiveTool(Tool *tool);

    void newFile();
    void clearCanvas();
    void selectColorDialog();

    Canvas *m_canvas;
    QSizeGrip *m_sizeGrip;
    QScrollArea *m_scrollArea;

    QColor m_activeColor;
    Tool *m_activeTool;

    QSharedPointer<BrushTool::Options> m_brushToolOptions;
    BrushTool *m_brushTool;
    QSharedPointer<LineTool::Options> m_lineToolOptions;
    LineTool *m_lineTool;
    QSharedPointer<FillTool::Options> m_fillToolOptions;
    FillTool *m_fillTool;

    QAction *m_newAction;
    QAction *m_clearAction;
    QAction *m_selectColorAction;
    QAction *m_brushToolAction;
    QAction *m_lineToolAction;
    QAction *m_fillToolAction;
    QActionGroup *m_colorActions;
};
#endif // ICGPAINT_H

#include "ICGPaint.h"

#include <QtWidgets>
#include <qactiongroup.h>
#include <qcolor.h>
#include <qnamespace.h>
#include <qwidget.h>

#include "Canvas.h"
#include "tool/BrushTool.h"
#include "tool/LineTool.h"
#include "tool/FillTool.h"

ICGPaint::ICGPaint() : QMainWindow()
{
    setWindowTitle("ICGPaint");
    setWindowIcon(QIcon(":resources/icons/icgpaint.svg"));
    createActions();

    m_canvas = new Canvas(QSize(800, 900));

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidget(m_canvas);
    setCentralWidget(m_scrollArea);

    setMinimumSize(600, 400);
    resize(minimumSize());

    m_brushToolOptions = QSharedPointer<BrushTool::Options>(new BrushTool::Options());
    m_brushTool = new BrushTool(m_canvas, m_brushToolOptions);

    m_lineToolOptions = QSharedPointer<LineTool::Options>(new LineTool::Options());
    m_lineTool = new LineTool(m_canvas, m_lineToolOptions);

    m_fillToolOptions = QSharedPointer<FillTool::Options>(new FillTool::Options());
    m_fillTool = new FillTool(m_canvas, m_fillToolOptions);

    m_activeTool = nullptr;
    setActiveTool(m_brushTool);

    setActiveColor(Qt::black);
}

void ICGPaint::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QMenu *toolMenu = menuBar()->addMenu(tr("&Tool"));
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QToolBar *toolbar = addToolBar(tr("File"));

    const QIcon newIcon = style()->standardIcon(QStyle::SP_FileIcon);
    const QIcon clearIcon = QIcon(":resources/icons/clear.svg");
    const QIcon selectColorIcon = generateColorIcon(Qt::black);

    m_newAction = new QAction(newIcon, tr("&New"), this);
    m_clearAction = new QAction(clearIcon, tr("&Clear"), this);
    m_selectColorAction = new QAction(selectColorIcon, tr("&Color"), this);

    m_brushToolAction = new QAction(BrushTool::Icon(), tr("&Brush"), this);
    m_lineToolAction = new QAction(LineTool::Icon(), tr("&Line"), this);
    m_fillToolAction = new QAction(FillTool::Icon(), tr("&Fill"), this);

    m_newAction->setShortcuts(QKeySequence::New);
    m_clearAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    m_selectColorAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_C));
    m_brushToolAction->setShortcut(QKeySequence(Qt::Key_B));
    m_lineToolAction->setShortcut(QKeySequence(Qt::Key_L));
    m_fillToolAction->setShortcut(QKeySequence(Qt::Key_F));

    m_newAction->setStatusTip("Create new file");
    m_clearAction->setStatusTip("Clear canvas");
    m_selectColorAction->setStatusTip("Select color");
    m_brushToolAction->setStatusTip("Draw with brush");
    m_lineToolAction->setStatusTip("Draw line");
    m_fillToolAction->setStatusTip("Fill area");

    m_brushToolAction->setCheckable(true);
    m_fillToolAction->setCheckable(true);
    m_lineToolAction->setCheckable(true);


    QActionGroup *toolActions = new QActionGroup(this);
    toolActions->addAction(m_brushToolAction);
    toolActions->addAction(m_fillToolAction);
    toolActions->addAction(m_lineToolAction);
    toolActions->setExclusive(true);

    fileMenu->addAction(m_newAction);
    fileMenu->addAction(m_clearAction);

    toolMenu->addAction(m_brushToolAction);
    toolMenu->addAction(m_lineToolAction);
    toolMenu->addAction(m_fillToolAction);
    toolMenu->addAction(m_selectColorAction);

    QAction *aboutAction = new QAction(windowIcon(), tr("&About"), this);
    helpMenu->addAction(aboutAction);

    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    toolbar->addAction(m_newAction);
    toolbar->addAction(m_clearAction);
    toolbar->addSeparator();
    toolbar->addAction(m_brushToolAction);
    toolbar->addAction(m_lineToolAction);
    toolbar->addAction(m_fillToolAction);
    toolbar->addSeparator();
    toolbar->addWidget(spacer);

    m_colorActions = createColorActions();
    for (QAction* action : m_colorActions->actions()) {
        toolbar->addAction(action);
    }
    toolbar->addSeparator();
    toolbar->addAction(m_selectColorAction);


    connect(m_newAction, &QAction::triggered, this, &ICGPaint::newFile);
    connect(m_clearAction, &QAction::triggered, this, &ICGPaint::clearCanvas);
    connect(m_selectColorAction, &QAction::triggered, this, &ICGPaint::selectColorDialog);
    connect(m_brushToolAction, &QAction::triggered, this, [this]() {
        setActiveTool(m_brushTool);
    });
    connect(m_lineToolAction, &QAction::triggered, this, [this]() {
        setActiveTool(m_lineTool);
    });
    connect(m_fillToolAction, &QAction::triggered, this, [this]() {
        setActiveTool(m_fillTool);
    });
    connect(aboutAction, &QAction::triggered, this, &ICGPaint::aboutDialog);
}

QActionGroup *ICGPaint::createColorActions()
{
    QList<QColor> colors = {
        QColor(Qt::black),      // Black
        QColor(Qt::white),      // White
        QColor(Qt::red),        // Red
        QColor(255, 165, 0),    // Orange
        QColor(Qt::yellow),     // Yellow
        QColor(Qt::green),      // Green
        QColor(Qt::blue),       // Blue
        QColor(75, 0, 130),     // Indigo
        QColor(238, 130, 238)   // Violet
    };

    QActionGroup *group = new QActionGroup(this);
    group->setExclusive(true);
    group->setExclusionPolicy(QActionGroup::ExclusionPolicy::ExclusiveOptional);

    for (const QColor& color : colors) {
        QAction *action = new QAction(generateColorIcon(color), color.name(), this);
        action->setCheckable(true);

        connect(action, &QAction::triggered, this, [color, this]() {
            setActiveColor(color);
        });

        group->addAction(action);
    }

    return group;
}

void ICGPaint::newFile()
{
    qInfo() << "New File clicked";
}

void ICGPaint::clearCanvas()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Clear canvas"), tr("Clear canvas?"));

    if (reply == QMessageBox::Yes) {
        m_canvas->clear();
    }
}

void ICGPaint::selectColorDialog()
{
    const QColor color = QColorDialog::getColor(m_activeColor, this, tr("Select Color"));

    if (color.isValid()) {
        setActiveColor(color);
        if (m_colorActions->checkedAction()) {
            m_colorActions->checkedAction()->setChecked(false);
        }
    }
    else {
        qDebug() << "Selected color is invalid";
    }
}

QIcon ICGPaint::generateColorIcon(QColor color)
{
    QPixmap pixmap(32, 32);
    pixmap.fill(color);
    return QIcon(pixmap);
}

void ICGPaint::setActiveColor(QColor color)
{
    m_activeColor = color;
    m_brushToolOptions->color = color;
    m_lineToolOptions->color = color;
    m_fillToolOptions->color = color;
    m_selectColorAction->setIcon(generateColorIcon(color));
}

void ICGPaint::setActiveTool(Tool *tool)
{
    tool->raise();
    m_activeTool = tool;
}

void ICGPaint::aboutDialog()
{
    QString text = "Made by Pavel Urdin\nGroup 22201 NSU FIT\n2025";
    QMessageBox::about(this, "About ICGPaint", text);

}

#include "ICGPaint.h"

#include <QtWidgets>

#include "Canvas.h"
#include "tool/BrushTool.h"

ICGPaint::ICGPaint() : QMainWindow()
{
    setWindowTitle("ICGPaint");
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

    m_activeTool = nullptr;
    setActiveTool(m_brushTool);
}

void ICGPaint::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));

    const QIcon newIcon = style()->standardIcon(QStyle::SP_FileIcon);
    const QIcon clearIcon = QIcon(":resources/icons/clear.svg");
    const QIcon selectColorIcon = generateColorIcon(Qt::black);

    m_newAction = new QAction(newIcon, tr("New"), this);
    m_clearAction = new QAction(clearIcon, tr("Clear"), this);
    m_selectColorAction = new QAction(selectColorIcon, tr("Color"), this);
    m_brushToolAction = new QAction(BrushTool::Icon(), tr("Brush"), this);
    m_lineToolAction = new QAction(LineTool::Icon(), tr("Line"), this);

    m_newAction->setShortcuts(QKeySequence::New);
    m_clearAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    m_selectColorAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_C));
    m_brushToolAction->setShortcut(QKeySequence(Qt::Key_B));
    m_lineToolAction->setShortcut(QKeySequence(Qt::Key_L));

    m_newAction->setStatusTip("Create new file");
    m_clearAction->setStatusTip("Clear canvas");
    m_selectColorAction->setStatusTip("Select color");
    m_brushToolAction->setStatusTip("Draw with brush");
    m_lineToolAction->setStatusTip("Draw line");

    fileMenu->addAction(m_newAction);
    fileMenu->addAction(m_clearAction);
    fileMenu->addAction(m_selectColorAction);
    fileMenu->addAction(m_brushToolAction);
    fileMenu->addAction(m_lineToolAction);

    fileToolBar->addAction(m_newAction);
    fileToolBar->addAction(m_clearAction);
    fileToolBar->addAction(m_selectColorAction);
    fileToolBar->addAction(m_brushToolAction);
    fileToolBar->addAction(m_lineToolAction);

    connect(m_newAction, &QAction::triggered, this, &ICGPaint::newFile);
    connect(m_clearAction, &QAction::triggered, this, &ICGPaint::clearCanvas);
    connect(m_selectColorAction, &QAction::triggered, this, &ICGPaint::selectColorDialog);
    connect(m_brushToolAction, &QAction::triggered, this, [this]() {
        setActiveTool(m_brushTool);
    });
    connect(m_lineToolAction, &QAction::triggered, this, [this]() {
        setActiveTool(m_lineTool);
    });
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
    m_selectColorAction->setIcon(generateColorIcon(color));
}

void ICGPaint::setActiveTool(Tool *tool)
{
    tool->raise();
    m_activeTool = tool;
}

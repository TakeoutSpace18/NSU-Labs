#include "ICGPaint.h"

#include <QtWidgets>
#include <QActionGroup>
#include <QBoxLayout>
#include <QColor>
#include <QDockWidget>
#include <QStackedLayout>
#include <QValidator>

#include "Canvas.h"
#include "ToolOptionsDialog.h"

#include "tool/BrushTool.h"
#include "tool/LineTool.h"
#include "tool/FillTool.h"
#include "tool/FillToolOptions.h"
#include "tool/BrushToolOptions.h"
#include "tool/LineToolOptions.h"

#include "tool/ToolOptionsPanel.h"

ICGPaint::ToolOptions::ToolOptions()
{
    brush = QSharedPointer<BrushToolOptions>(new BrushToolOptions());
    line = QSharedPointer<LineToolOptions>(new LineToolOptions());
    fill = QSharedPointer<FillToolOptions>(new FillToolOptions());
}

ICGPaint::ICGPaint() : QMainWindow()
{
    setWindowTitle("ICGPaint");
    setWindowIcon(QIcon(":resources/icons/icgpaint.svg"));

    m_activeTool = nullptr;
    m_toolsLayout = nullptr;

    m_scrollArea = new QScrollArea(this);
    setCentralWidget(m_scrollArea);

    setCanvas(new Canvas(QSize(640, 480), this));

    createActions();
    createToolOptionsDock();
    createTools(m_canvas);

    setMinimumSize(600, 400);
    resize(minimumSize());

    setActiveColor(Qt::black);
}

void ICGPaint::createTools(Canvas *canvas)
{
    m_toolOptions = ToolOptions();


    m_brushTool = new BrushTool(canvas, m_toolOptions.brush);
    m_lineTool = new LineTool(canvas, m_toolOptions.line);
    m_fillTool = new FillTool(canvas, m_toolOptions.fill);

    m_tools.append(m_brushTool);
    m_tools.append(m_lineTool);
    m_tools.append(m_fillTool);

    m_toolsLayout = new QStackedLayout;
    for (auto tool : m_tools) {
        m_toolsLayout->addWidget(tool);
    }

    canvas->setLayout(m_toolsLayout);

    m_activeTool = nullptr;
    m_brushToolAction->trigger();
}

void ICGPaint::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QMenu *toolMenu = menuBar()->addMenu(tr("&Tool"));
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QToolBar *toolbar = addToolBar(tr("File"));

    const QIcon newIcon = style()->standardIcon(QStyle::SP_FileIcon);
    const QIcon saveIcon = style()->standardIcon(QStyle::SP_DialogSaveButton);
    const QIcon openIcon = style()->standardIcon(QStyle::SP_DialogOpenButton);
    const QIcon clearIcon = QIcon(":resources/icons/clear.svg");
    const QIcon selectColorIcon = generateColorIcon(Qt::black);
    const QIcon optionsIcon = QIcon(":resources/icons/options.svg");

    m_newAction = new QAction(newIcon, tr("&New"), this);
    m_saveAction = new QAction(saveIcon, tr("&Save"), this);
    m_openAction = new QAction(openIcon, tr("&Open"), this);
    m_clearAction = new QAction(clearIcon, tr("&Clear"), this);
    m_selectColorAction = new QAction(selectColorIcon, tr("&Color"), this);
    m_toolOptionsAction = new QAction(optionsIcon, tr("Tool &Options"), this);

    m_brushToolAction = new QAction(BrushTool::Icon(), tr("&Brush"), this);
    m_lineToolAction = new QAction(LineTool::Icon(), tr("&Line"), this);
    m_fillToolAction = new QAction(FillTool::Icon(), tr("&Fill"), this);

    m_newAction->setShortcuts(QKeySequence::New);
    m_saveAction->setShortcuts(QKeySequence::Save);
    m_openAction->setShortcuts(QKeySequence::Open);
    m_clearAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    m_selectColorAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_C));
    m_brushToolAction->setShortcut(QKeySequence(Qt::Key_B));
    m_lineToolAction->setShortcut(QKeySequence(Qt::Key_L));
    m_fillToolAction->setShortcut(QKeySequence(Qt::Key_F));
    m_toolOptionsAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_P));

    m_newAction->setStatusTip("Create new file");
    m_saveAction->setStatusTip("Save file");
    m_openAction->setStatusTip("Open file");
    m_clearAction->setStatusTip("Clear canvas");
    m_selectColorAction->setStatusTip("Select color");
    m_brushToolAction->setStatusTip("Draw with brush");
    m_lineToolAction->setStatusTip("Draw line");
    m_fillToolAction->setStatusTip("Fill area");
    m_fillToolAction->setStatusTip("Tool options");

    m_brushToolAction->setCheckable(true);
    m_fillToolAction->setCheckable(true);
    m_lineToolAction->setCheckable(true);

    QActionGroup *toolActions = new QActionGroup(this);
    toolActions->addAction(m_brushToolAction);
    toolActions->addAction(m_fillToolAction);
    toolActions->addAction(m_lineToolAction);
    toolActions->setExclusive(true);

    fileMenu->addAction(m_newAction);
    fileMenu->addAction(m_openAction);
    fileMenu->addAction(m_saveAction);

    toolMenu->addAction(m_clearAction);
    toolMenu->addAction(m_brushToolAction);
    toolMenu->addAction(m_lineToolAction);
    toolMenu->addAction(m_fillToolAction);
    toolMenu->addAction(m_selectColorAction);
    toolMenu->addAction(m_toolOptionsAction);

    QAction *aboutAction = new QAction(windowIcon(), tr("&About"), this);
    helpMenu->addAction(aboutAction);

    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    toolbar->addAction(m_newAction);
    toolbar->addAction(m_openAction);
    toolbar->addAction(m_saveAction);
    toolbar->addSeparator();

    toolbar->addAction(m_clearAction);
    toolbar->addAction(m_toolOptionsAction);
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

    connect(m_newAction, &QAction::triggered, this, &ICGPaint::newFileDialog);
    connect(m_saveAction, &QAction::triggered, this, &ICGPaint::saveFile);
    connect(m_openAction, &QAction::triggered, this, &ICGPaint::openFile);
    connect(m_clearAction, &QAction::triggered, this, &ICGPaint::clearCanvas);
    connect(m_toolOptionsAction, &QAction::triggered, this, &ICGPaint::toolOptions);
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

void ICGPaint::createToolOptionsDock()
{
    m_toolOptionsDock = new QDockWidget("Tool options", this);
    m_toolOptionsDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    addDockWidget(Qt::RightDockWidgetArea, m_toolOptionsDock);
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

void ICGPaint::setCanvas(Canvas *canvas)
{
    // move all tool widgets to new canvas
    for (auto tool : m_tools) {
        tool->setCanvas(canvas);
    }

    if (m_toolsLayout)
        canvas->setLayout(m_toolsLayout);
    m_scrollArea->setWidget(canvas);
    m_canvas = canvas;
}

void ICGPaint::newFileDialog()
{
    QDialog dialog;
    dialog.setWindowTitle("Create new file");
    dialog.setModal(true);
    dialog.setFixedSize(QSize(200, 150));

    QVBoxLayout layout(&dialog);
    QFormLayout formLayout;

    QLineEdit widthEdit("640"), heightEdit("480");
    QValidator *validator = new QIntValidator(10, 20000, &dialog);
    widthEdit.setValidator(validator);
    heightEdit.setValidator(validator);
    formLayout.addRow("Width:", &widthEdit);
    formLayout.addRow("Height:", &heightEdit);

    QPushButton okButton("OK"), cancelButton("Cancel");

    QObject::connect(&okButton, &QPushButton::clicked, [&]() { dialog.accept(); });
    QObject::connect(&cancelButton, &QPushButton::clicked, [&]() { dialog.reject(); });

    layout.addLayout(&formLayout);
    layout.addWidget(&okButton);
    layout.addWidget(&cancelButton);

    if (dialog.exec() == QDialog::Accepted) {
        int width = widthEdit.text().toInt();
        int height = heightEdit.text().toInt();

        Canvas *canvas = new Canvas(QSize(width, height));
        setCanvas(canvas);

        m_savePath = "";
    }
}

void ICGPaint::saveFile()
{
    if (m_savePath.isEmpty()) {
        m_savePath = QFileDialog::getSaveFileName(this, "Save file",
                                     QDir::homePath() + "/untitled.png",
                                     "PNG Image (*.png);;All Files (*)");
    }

    if (!m_savePath.isEmpty() && !m_canvas->image().save(m_savePath, "PNG")) {
        qWarning() << "Failed to save to " << m_savePath;
    }
}

void ICGPaint::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open file"),
                                                    QDir::homePath(),
                                                    tr("Image Files (*.png *.jpg *.jpeg *.bmp);;All Files (*)"));
    Canvas *canvas = new Canvas(QImage(filePath));
    setCanvas(canvas);
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
    m_toolOptions.line->setColor(color);
    m_toolOptions.brush->setColor(color);
    m_toolOptions.fill->setColor(color);
    m_selectColorAction->setIcon(generateColorIcon(color));
}

void ICGPaint::setActiveTool(Tool *tool)
{
    m_toolsLayout->setCurrentWidget(tool);
    m_activeTool = tool;

    ToolOptionsPanel *optionsPanel = tool->createOptionsPanel(this);
    if (optionsPanel) {
        optionsPanel->setAutoApply(true);
        m_toolOptionsDock->show();
        m_toolOptionsDock->setWidget(optionsPanel);
    }
    else {
        m_toolOptionsDock->hide();
    }
}

void ICGPaint::aboutDialog()
{
    // TODO: expand this text
    QString text = "Made by Pavel Urdin\nGroup 22201 NSU FIT\n2025";
    QMessageBox::about(this, "About ICGPaint", text);
}

void ICGPaint::toolOptions()
{
    ToolOptionsDialog dialog(m_toolOptions, this);
    dialog.exec();
}

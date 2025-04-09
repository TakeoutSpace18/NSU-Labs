#include "ICGFilter.h"

#include <QAction>
#include <QActionGroup>
#include <QCursor>
#include <QDir>
#include <QFileDialog>
#include <QFuture>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QMessageBox>
#include <QSharedPointer>
#include <QStatusBar>
#include <QWidgetAction>
#include <QtConcurrent/QtConcurrent>

#include "./ui_ICGFilter.h"
#include "AboutDialog.h"
#include "ImageViewport.h"
#include "filter/BWFilter.h"
#include "filter/BlurFilter.h"
#include "filter/EmbossFilter.h"
#include "filter/Filter.h"
#include "filter/FilterPanel.h"
#include "filter/GammaCorrectionFilter.h"
#include "filter/InversionFilter.h"
#include "filter/SharpenFilter.h"
#include "filter/EdgeDetectionFilter.h"
#include "filter/DitheringFilter.h"
#include "filter/RotationFilter.h"
#include "filter/WatercolorFilter.h"
#include "filter/ChromaticAberrationFilter.h"

ICGFilter::ICGFilter(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::ICGFilter)
{
    ui->setupUi(this);
    ui->dockWidget->hide();
    ui->viewport->setDragMode(QGraphicsView::DragMode::NoDrag);

    createFilters();
    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->toolBar->addWidget(spacer);
    createTransformationModeChoice();

    setWindowTitle("ICGFilter");
    setWindowIcon(QIcon(":resources/icons/image-edit.svg"));

    connect(ui->viewport, &ImageViewport::scaleChanged, this,
            [this](double scale) {
                ui->fitToViewportAction->setChecked(false);
                ui->originalSizeAction->setChecked(false);
                statusBar()->showMessage("Scale: " + QString::number(scale));
            });

    setupMouseTogglePreviewOriginal();

    ui->handAction->trigger();

    importImage(":resources/test-image.jpg");
}

ICGFilter::~ICGFilter()
{
    delete ui;
}

void ICGFilter::setupMouseTogglePreviewOriginal()
{
    connect(ui->viewport, &ImageViewport::mousePressed, this,
            &ICGFilter::togglePreviewOriginal);
    connect(ui->viewport, &ImageViewport::mouseReleased, this,
            &ICGFilter::togglePreviewOriginal);
    connect(ui->viewport, &ImageViewport::mouseDblPressed, this,
            &ICGFilter::togglePreviewOriginal);
}

void ICGFilter::createTransformationModeChoice()
{
    QWidget *comboWidget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(comboWidget);
    layout->setContentsMargins(0, 0, 0, 0);

    QLabel *label = new QLabel("Interpolation mode:");
    QComboBox *comboBox = new QComboBox();
    comboBox->addItems({"Fast", "Smooth"});

    connect(
        comboBox, &QComboBox::currentTextChanged, this,
        [=](const QString &text) {
            if (text == "Fast") {
                ui->viewport->setTransformationMode(Qt::FastTransformation);
                ui->statusBar->showMessage("Fast transformation mode");
            } else if (text == "Smooth") {
                ui->viewport->setTransformationMode(Qt::SmoothTransformation);
                ui->statusBar->showMessage("Smooth transformation mode");
            }
        });

    layout->addWidget(label);
    layout->addWidget(comboBox);

    ui->toolBar->addWidget(comboWidget);
}

void ICGFilter::on_saveAction_triggered()
{
    if (savePath.isEmpty()) {
        savePath = QFileDialog::getSaveFileName(
            this, "Save file", QDir::homePath() + "/untitled.png",
            "PNG Image (*.png);;All Files (*)");
    }

    if (savePath.isEmpty() || !saveImage(savePath)) {
        savePath = "";
    }
}

void ICGFilter::on_importAction_triggered()
{
    QString filter =
        tr("Image Files (*.png *.jpg *.jpeg *.bmp);;All Files (*)");
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open file"),
                                                    QDir::homePath(), filter);

    if (!filePath.isEmpty()) {
        importImage(filePath);
    }
}

void ICGFilter::on_aboutAction_triggered()
{
    AboutDialog *dialog = new AboutDialog(this);
    dialog->exec();
}

bool ICGFilter::importImage(const QString &path)
{
    QImage image = QImage(path);
    if (image.isNull()) {
        statusBar()->showMessage("Falied to import " + path);
        QMessageBox messageBox(QMessageBox::Critical, "Falied to import image",
                               "Falied to import image " + path,
                               QMessageBox::Ok, this);
        messageBox.exec();
        return false;
    }

    original = image;
    preview = original;
    ui->viewport->updateImage(preview);

    if (!activeFilter.isNull()) {
        previewFilter(*activeFilter);
    }

    statusBar()->showMessage("Imported " + path);

    return true;
}

bool ICGFilter::saveImage(const QString &path)
{
    if (!original.save(path, "PNG")) {
        statusBar()->showMessage("Falied to save to " + path);
        QMessageBox messageBox(QMessageBox::Critical, "Falied to save image",
                               "Falied to save image to " + path,
                               QMessageBox::Ok, this);
        messageBox.exec();
        return false;
    }

    statusBar()->showMessage("Saved to " + path);
    return true;
}

void ICGFilter::on_originalSizeAction_triggered()
{
    ui->viewport->originalSize();
    ui->originalSizeAction->setChecked(true);

    ui->fitToViewportAction->setChecked(false);

    statusBar()->showMessage("Original size");
}

void ICGFilter::on_fitToViewportAction_triggered()
{
    ui->viewport->fitToViewport();
    ui->fitToViewportAction->setChecked(true);

    ui->originalSizeAction->setChecked(false);

    statusBar()->showMessage("Fit to viewport");
}

void ICGFilter::resizeEvent(QResizeEvent *event)
{
    ui->fitToViewportAction->setChecked(false);
    ui->originalSizeAction->setChecked(false);
    statusBar()->clearMessage();

    QWidget::resizeEvent(event);
}

void ICGFilter::addFilter(QSharedPointer<Filter> filter)
{
    filters.append(filter);

    QAction *action = filter->createAction(this);
    action->setData(QVariant::fromValue(filter));
    action->setCheckable(true);

    filterActions->addAction(action);
    ui->toolBar->addAction(action);
    ui->menuFilter->addAction(action);

    connect(action, &QAction::triggered, this, [this]() {
        QAction *action = qobject_cast<QAction *>(sender());
        if (action->isChecked()) {
            setActiveFilter(action->data().value<QSharedPointer<Filter>>());
        } else {
            cancelFilter();
        }
    });
}

void ICGFilter::setActiveFilter(QSharedPointer<Filter> filter)
{
    preview = original;
    ui->viewport->updateImage(original);

    FilterPanel *panel = new FilterPanel(filter, this);
    ui->dockWidget->setWidget(panel);
    ui->dockWidget->setWindowTitle(filter->getDisplayName());
    ui->dockWidget->show();

    connect(panel, &FilterPanel::panelClosed, this,
            [this]() { cancelFilter(); });

    connect(panel, &FilterPanel::applyFilter, this, [this]() {
        FilterPanel *panel = qobject_cast<FilterPanel *>(sender());
        QSharedPointer<Filter> filter = panel->getFilter();
        applyFilter(*filter.get());
    });

    connect(panel, &FilterPanel::previewFilter, this, [this]() {
        FilterPanel *panel = qobject_cast<FilterPanel *>(sender());
        QSharedPointer<Filter> filter = panel->getFilter();
        previewFilter(*filter.get());
    });

    if (panel->isAutoPreviewEnabled()) {
        previewFilter(*filter.get());
    }

    activeFilter = filter;
}

void ICGFilter::createFilters()
{
    filterActions = new QActionGroup(this);
    filterActions->setExclusionPolicy(
        QActionGroup::ExclusionPolicy::ExclusiveOptional);

    addFilter(QSharedPointer<BWFilter>::create());
    addFilter(QSharedPointer<InversionFilter>::create());
    addFilter(QSharedPointer<BlurFilter>::create());
    addFilter(QSharedPointer<SharpenFilter>::create());
    addFilter(QSharedPointer<EmbossFilter>::create());
    addFilter(QSharedPointer<GammaCorrectionFilter>::create());
    addFilter(QSharedPointer<EdgeDetectionFilter>::create());
    addFilter(QSharedPointer<DitheringFilter>::create());
    addFilter(QSharedPointer<RotationFilter>::create());
    addFilter(QSharedPointer<WatercolorFilter>::create());
    addFilter(QSharedPointer<ChromaticAberrationFilter>::create());
}

void ICGFilter::applyFilter(const Filter &filter)
{
    previewFilter(filter);
    original = preview;

    ui->statusBar->showMessage("Applied " + filter.getDisplayName());
    cancelFilter();
}

void ICGFilter::previewFilter(const Filter &filter)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    filter.apply(original, preview);
    ui->viewport->updateImage(preview);
    QApplication::restoreOverrideCursor();
}

void ICGFilter::cancelFilter()
{
    for (QAction *action : filterActions->actions()) {
        action->setChecked(false);
    }
    ui->dockWidget->hide();

    ui->viewport->updateImage(original);
    preview = original;
}

void ICGFilter::on_handAction_triggered()
{
    if (ui->handAction->isChecked()) {
        ui->viewport->setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
    } else {
        ui->viewport->setDragMode(QGraphicsView::DragMode::NoDrag);
    }
}

void ICGFilter::togglePreviewOriginal()
{
    // Disable orignal preview toggle when hand dragging is active
    if (ui->handAction->isChecked()) {
        return;
    }

    showOriginal = !showOriginal;
    if (showOriginal) {
        ui->viewport->updateImage(original);
        ui->statusBar->showMessage("Show original");
    } else {
        ui->viewport->updateImage(preview);
        ui->statusBar->showMessage("Show preview");
    }
}

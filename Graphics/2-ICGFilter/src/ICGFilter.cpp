#include "ICGFilter.h"
#include "./ui_ICGFilter.h"

#include <QFileDialog>
#include <QDir>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QStatusBar>
#include <QMessageBox>
#include <qmessagebox.h>

#include "AboutDialog.h"
#include "ImageViewport.h"

ICGFilter::ICGFilter(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::ICGFilter)
{
    ui->setupUi(this);
    setWindowTitle("ICGFilter");
    setWindowIcon(QIcon(":resources/icons/image-edit.svg"));

    connect(ui->viewport, &ImageViewport::scaleChanged, this, [this](double scale) {
        ui->fitToViewportAction->setChecked(false);
        ui->originalSizeAction->setChecked(false);
        statusBar()->showMessage("Scale: " + QString::number(scale));
    });

    importImage(":resources/test-image.jpg");
}

ICGFilter::~ICGFilter()
{
    delete ui;
}

void ICGFilter::on_saveAction_triggered()
{
    if (savePath.isEmpty()) {
        savePath = QFileDialog::getSaveFileName(this, "Save file",
                                     QDir::homePath() + "/untitled.png",
                                     "PNG Image (*.png);;All Files (*)");
    }

    if (savePath.isEmpty() || !saveImage(savePath)) {
        savePath = "";
    }
}


void ICGFilter::on_importAction_triggered()
{
    QString filter = tr("Image Files (*.png *.jpg *.jpeg *.bmp);;All Files (*)");
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Open file"),
                                                    QDir::homePath(),
                                                    filter);

    if (!filePath.isEmpty()) {
        importImage(filePath);
    }
}


void ICGFilter::on_aboutAction_triggered()
{
    AboutDialog *dialog = new AboutDialog(this);
    dialog->exec();
}

bool ICGFilter::importImage(const QString& path)
{
    QImage image = QImage(path);
    if (image.isNull()) {
        statusBar()->showMessage("Falied to import " + path);
        QMessageBox messageBox(QMessageBox::Critical,
                               "Falied to import image",
                               "Falied to import image " + path,
                               QMessageBox::Ok,
                               this);
        messageBox.exec();
        return false;
    }

    original = image;
    preview = original;
    ui->viewport->updateImage(preview);

    statusBar()->showMessage("Imported " + path);

    return true;
}

bool ICGFilter::saveImage(const QString& path)
{
    if (!original.save(path, "PNG")) {
        statusBar()->showMessage("Falied to save to " + path);
        QMessageBox messageBox(QMessageBox::Critical,
                               "Falied to save image",
                               "Falied to save image to " + path,
                               QMessageBox::Ok,
                               this);
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


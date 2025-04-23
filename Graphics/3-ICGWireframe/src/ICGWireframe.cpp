#include "ICGWireframe.h"

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
#include <qsharedpointer.h>

#include "./ui_ICGWireframe.h"
#include "AboutDialog.h"
#include "BSplineEditor.h"

ICGWireframe::ICGWireframe(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::ICGWireframe)
{
    ui->setupUi(this);

    setWindowTitle("ICGWireframe");
    setWindowIcon(QIcon(":resources/icons/sphere.svg"));
}

ICGWireframe::~ICGWireframe()
{
    delete ui;
}

void ICGWireframe::on_saveAction_triggered()
{
    if (savePath.isEmpty()) {
        savePath = QFileDialog::getSaveFileName(
            this, "Save file", QDir::homePath() + "/untitled.json",
            "Json file (*.json);;All Files (*)");
    }

    if (savePath.isEmpty() || !saveScene(savePath)) {
        savePath = "";
    }
}

void ICGWireframe::on_importAction_triggered()
{
    QString filter =
        tr("Json Files (*.json);;All Files (*)");
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open file"),
                                                    QDir::homePath(), filter);

    if (!filePath.isEmpty()) {
        importScene(filePath);
    }
}

void ICGWireframe::on_aboutAction_triggered()
{
    AboutDialog *dialog = new AboutDialog(this);
    dialog->exec();
}

bool ICGWireframe::importScene(const QString &path)
{

    // if (...) {
    //     statusBar()->showMessage("Falied to import " + path);
    //     QMessageBox messageBox(QMessageBox::Critical, "Falied to import scene",
    //                            "Falied to import scene " + path,
    //                            QMessageBox::Ok, this);
    //     messageBox.exec();
    //     return false;
    // }
    
    // TODO; import logic

    statusBar()->showMessage("Imported " + path);

    return true;
}

bool ICGWireframe::saveScene(const QString &path)
{
    // TODO: save logic
    // if (!original.save(path, "PNG")) {
    //     statusBar()->showMessage("Falied to save to " + path);
    //     QMessageBox messageBox(QMessageBox::Critical, "Falied to save image",
    //                            "Falied to save image to " + path,
    //                            QMessageBox::Ok, this);
    //     messageBox.exec();
    //     return false;
    // }

    statusBar()->showMessage("Saved to " + path);
    return true;
}

void ICGWireframe::resizeEvent(QResizeEvent *event)
{
    // TODO 
    // ui->fitToViewportAction->setChecked(false);
    // ui->originalSizeAction->setChecked(false);
    statusBar()->clearMessage();

    QWidget::resizeEvent(event);
}

void ICGWireframe::on_editCurveAction_toggled(bool checked)
{
    if (checked) {
        openBSplineEditor();
    }
    else {
        closeBSplineEditor();
    }
}

void ICGWireframe::openBSplineEditor()
{
    if (bSplineEditor) {
        return;
    }

    bSplineEditor = new BSplineEditor(this);
    connect(bSplineEditor, &QDialog::finished, this, &ICGWireframe::closeBSplineEditor);

    ui->dockWidget->setWidget(bSplineEditor);
    ui->dockWidget->show();
    ui->editCurveAction->setChecked(true);
}

void ICGWireframe::closeBSplineEditor()
{
    ui->dockWidget->hide();
    ui->editCurveAction->setChecked(false);
    delete bSplineEditor;
    bSplineEditor = nullptr;
}

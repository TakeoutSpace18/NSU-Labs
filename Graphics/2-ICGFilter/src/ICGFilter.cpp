#include "ICGFilter.h"
#include "./ui_ICGFilter.h"

#include <QFileDialog>

#include "AboutDialog.h"

ICGFilter::ICGFilter(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ICGFilter)
{
    ui->setupUi(this);
    setWindowTitle("ICGFilter");
    setWindowIcon(QIcon(":resources/icons/image-edit.svg"));
}

ICGFilter::~ICGFilter()
{
    delete ui;
}

void ICGFilter::on_saveAction_triggered()
{
    if (m_savePath.isEmpty()) {
        m_savePath = QFileDialog::getSaveFileName(this, "Save file",
                                     QDir::homePath() + "/untitled.png",
                                     "PNG Image (*.png);;All Files (*)");
    }

    // TODO: save file 
    // if (!m_savePath.isEmpty() && !m_canvas->image().save(m_savePath, "PNG")) {
    //     qWarning() << "Failed to save to " << m_savePath;
    // }
}


void ICGFilter::on_importAction_triggered()
{
    QString filter = tr("Image Files (*.png *.jpg *.jpeg *.bmp);;All Files (*)");
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open file"), QDir::homePath(), filter);

    // TODO: open file
    // Canvas *canvas = new Canvas(QImage(filePath));
}


void ICGFilter::on_aboutAction_triggered()
{
    AboutDialog *dialog = new AboutDialog(this);
    dialog->exec();
}


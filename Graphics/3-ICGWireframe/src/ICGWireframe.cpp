#include "ICGWireframe.h"

#include <qglobal.h>

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
#include <stdexcept>

#include "./ui_ICGWireframe.h"
#include "AboutDialog.h"
#include "RotationFigure.h"
#include "SceneData.h"
#include "spline_editor/SplineEditor.h"

ICGWireframe::ICGWireframe(QWidget *parent)
    : QMainWindow(parent), m_sceneData(new SceneData), ui(new Ui::ICGWireframe)
{
    ui->setupUi(this);
    ui->dockWidget->hide();

    setWindowTitle("ICGWireframe");
    setWindowIcon(QIcon(":resources/icons/sphere.svg"));

    connect(ui->sceneViewport, &SceneViewport::zoomChanged, this,
            [this](qreal zoom) { m_sceneData->setZoom(zoom); });

    connect(ui->sceneViewport, &SceneViewport::rotationChanged, this,
            [this](QVector3D rotation) { m_sceneData->setRotation(rotation); });

    importScene(":resources/defaultScene.json");

    updateViewport(m_sceneData);
}

ICGWireframe::~ICGWireframe()
{
    delete ui;
}

void ICGWireframe::on_saveAction_triggered()
{
    if (m_savePath.isEmpty()) {
        m_savePath = QFileDialog::getSaveFileName(
            this, "Save file", QDir::homePath() + "/untitled.json",
            "Json file (*.json);;All Files (*)");
    }

    if (m_savePath.isEmpty() || !saveScene(m_savePath)) {
        m_savePath = "";
    }
}

void ICGWireframe::on_importAction_triggered()
{
    QString filter = tr("Json Files (*.json);;All Files (*)");
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
    auto errorMsg = [&](const QString &msg) {
        statusBar()->showMessage("Falied to import " + path);
        QMessageBox messageBox(
            QMessageBox::Critical, "Falied to import scene",
            "Falied to import scene " + path + "(" + msg + ")", QMessageBox::Ok,
            this);
        messageBox.exec();
    };

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorMsg("Cannot open file");
        return false;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

    try {
        m_sceneData = SceneData::FromJson(doc.object());
    }
    catch (const std::invalid_argument &e) {
        errorMsg(e.what());
        return false;
    }

    closeBSplineEditor();
    updateViewport(m_sceneData);
    ui->sceneViewport->setRotation(m_sceneData->rotation());
    ui->sceneViewport->setZoom(m_sceneData->zoom());

    statusBar()->showMessage("Imported " + path);

    return true;
}

bool ICGWireframe::saveScene(const QString &path)
{
    QJsonDocument doc(m_sceneData->toJson());

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        statusBar()->showMessage("Falied to save to " + path);
        QMessageBox messageBox(QMessageBox::Critical, "Falied to save image",
                               "Falied to save image to " + path +
                                   "(Cannot open file for writing)",
                               QMessageBox::Ok, this);
        messageBox.exec();
        return false;
        return false;
    }

    file.write(doc.toJson(QJsonDocument::Indented));

    file.close();
    statusBar()->showMessage("Saved to " + path);
    return true;
}

void ICGWireframe::resizeEvent(QResizeEvent *event)
{
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
    if (m_splineEditor) {
        return;
    }

    m_splineEditor = new SplineEditor(m_sceneData, this);
    connect(m_splineEditor, &QDialog::finished, this,
            &ICGWireframe::closeBSplineEditor);

    connect(m_splineEditor, &SplineEditor::apply, this,
            &ICGWireframe::updateViewport);

    ui->dockWidget->setWidget(m_splineEditor);
    ui->dockWidget->show();
    ui->editCurveAction->setChecked(true);
}

void ICGWireframe::closeBSplineEditor()
{
    ui->dockWidget->hide();
    ui->editCurveAction->setChecked(false);
    delete m_splineEditor;
    m_splineEditor = nullptr;
}

void ICGWireframe::updateViewport(QSharedPointer<SceneData> sceneData)
{
    ui->sceneViewport->setSceneVertices(RotationFigure::Generate(*sceneData));
}

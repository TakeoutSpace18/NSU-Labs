#ifndef ICGWIREFRAME_H
#define ICGWIREFRAME_H

#include <qsharedpointer.h>
#include <QAction>
#include <QGraphicsScene>
#include <QMainWindow>
#include <QSharedPointer>

#include "spline_editor/SplineEditor.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class ICGWireframe;
}
QT_END_NAMESPACE

class ICGWireframe : public QMainWindow
{
    Q_OBJECT

public:
    ICGWireframe(QWidget* parent = nullptr);
    ~ICGWireframe();

    bool saveScene(const QString& path);
    bool importScene(const QString& path);

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void on_saveAction_triggered();
    void on_importAction_triggered();
    void on_aboutAction_triggered();
    void on_editCurveAction_toggled(bool checked);

private:
    void openBSplineEditor();
    void closeBSplineEditor();

    void updateViewport(QSharedPointer<SceneData> sceneData);

    QSharedPointer<SceneData> m_sceneData;
    SplineEditor *m_splineEditor = nullptr;

    QString m_savePath;

    Ui::ICGWireframe* ui;
};
#endif  // ICGWIREFRAME_H

#ifndef ICGWIREFRAME_H
#define ICGWIREFRAME_H

#include "BSplineEditor.h"
#include <QAction>
#include <QGraphicsScene>
#include <QMainWindow>
#include <QSharedPointer>

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

    BSplineEditor *bSplineEditor = nullptr;

    Ui::ICGWireframe* ui;
    QString savePath;
};
#endif  // ICGWIREFRAME_H

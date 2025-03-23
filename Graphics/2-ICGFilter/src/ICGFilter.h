#ifndef ICGFILTER_H
#define ICGFILTER_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <qgraphicsscene.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class ICGFilter;
}
QT_END_NAMESPACE

class ICGFilter : public QMainWindow
{
    Q_OBJECT

public:
    ICGFilter(QWidget *parent = nullptr);
    ~ICGFilter();

    bool importImage(const QString& path);
    bool saveImage(const QString& path);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_saveAction_triggered();

    void on_importAction_triggered();

    void on_aboutAction_triggered();

    void on_originalSizeAction_triggered();

    void on_fitToViewportAction_triggered();

private:
    Ui::ICGFilter *ui;

    QImage original;
    QImage preview;

    QString savePath;
};
#endif // ICGFILTER_H

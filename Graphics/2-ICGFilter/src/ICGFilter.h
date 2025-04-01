#ifndef ICGFILTER_H
#define ICGFILTER_H

#include <QAction>
#include <QGraphicsScene>
#include <QMainWindow>
#include <QSharedPointer>

#include "filter/Filter.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class ICGFilter;
}
QT_END_NAMESPACE

class ICGFilter : public QMainWindow
{
    Q_OBJECT

public:
    ICGFilter(QWidget* parent = nullptr);
    ~ICGFilter();

    bool importImage(const QString& path);
    bool saveImage(const QString& path);

    void setActiveFilter(QSharedPointer<Filter> filter);

    void applyFilter(const Filter& filter);
    void previewFilter(const Filter& filter);
    void cancelFilter();

    void togglePreviewOriginal();

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void on_saveAction_triggered();

    void on_importAction_triggered();

    void on_aboutAction_triggered();

    void on_originalSizeAction_triggered();

    void on_fitToViewportAction_triggered();

    void on_handAction_triggered();

private:
    void createFilters();
    void addFilter(QSharedPointer<Filter> filter);
    void createTransformationModeChoice();
    void setupMouseTogglePreviewOriginal();

    Ui::ICGFilter* ui;

    QList<QSharedPointer<Filter>> filters;
    QSharedPointer<Filter> activeFilter;

    QActionGroup* filterActions;

    QImage original;
    QImage preview;

    QString savePath;

    bool showOriginal = false;
};
#endif  // ICGFILTER_H

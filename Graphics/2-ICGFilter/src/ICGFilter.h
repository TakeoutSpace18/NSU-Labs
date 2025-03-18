#ifndef ICGFILTER_H
#define ICGFILTER_H

#include <QMainWindow>

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

private slots:
    void on_saveAction_triggered();

    void on_importAction_triggered();

    void on_aboutAction_triggered();

private:
    Ui::ICGFilter *ui;

    QString m_savePath;
};
#endif // ICGFILTER_H

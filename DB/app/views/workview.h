#ifndef WORKVIEW_H
#define WORKVIEW_H

#include "baseview.h"
#include <QTabWidget>

class WorkView : public QWidget
{
    Q_OBJECT
    
public:
    explicit WorkView(QWidget *parent = nullptr);
    void setDatabase(const QSqlDatabase &database);
    
private:
    QTabWidget *tabWidget;
    BaseView *workScheduleView;
    BaseView *workTypeView;
};

#endif // WORKVIEW_H
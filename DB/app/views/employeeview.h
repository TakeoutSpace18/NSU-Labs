#ifndef EMPLOYEEVIEW_H
#define EMPLOYEEVIEW_H

#include "baseview.h"
#include <QTabWidget>

class EmployeeView : public QWidget
{
    Q_OBJECT
    
public:
    explicit EmployeeView(QWidget *parent = nullptr);
    void setDatabase(const QSqlDatabase &database);
    
private:
    QTabWidget *tabWidget;
    BaseView *employeeGeneralView;
    BaseView *workerView;
    BaseView *specialistView;
    BaseView *gradeView;
    BaseView *workerTypeView;
    BaseView *specialistTypeView;
};

#endif // EMPLOYEEVIEW_H
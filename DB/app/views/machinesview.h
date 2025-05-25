#ifndef MACHINESVIEW_H
#define MACHINESVIEW_H

#include "baseview.h"
#include <QTabWidget>

class MachinesView : public QWidget
{
    Q_OBJECT
    
public:
    explicit MachinesView(QWidget *parent = nullptr);
    void setDatabase(const QSqlDatabase &database);
    
private:
    QTabWidget *tabWidget;
    BaseView *machineView;
    BaseView *machineTypeView;
    BaseView *machineModelView;
    BaseView *siteMachineView;
};

#endif // MACHINESVIEW_H
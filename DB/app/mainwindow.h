#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QSqlDatabase>
#include <QSqlError>
#include <QPushButton>
#include <QMessageBox>

#include "views/departmentview.h"
#include "views/siteview.h"
#include "views/customerview.h"
#include "views/objectview.h"
#include "views/employeeview.h"
#include "views/brigadesview.h"
#include "views/workview.h"
#include "views/materialsview.h"
#include "views/machinesview.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    void setDatabase(const QSqlDatabase &database);

private slots:
    void about();
    
private:
    void createMenus();
    void createStatusBar();
    void setupTabs();
    
    QTabWidget *tabWidget;
    QSqlDatabase db;
    
    // Views for each major section
    DepartmentView *departmentView;
    SiteView *siteView;
    CustomerView *customerView;
    ObjectView *objectView;
    EmployeeView *employeeView;
    BrigadesView *brigadesView;
    WorkView *workView;
    MaterialsView *materialsView;
    MachinesView *machinesView;
};

#endif // MAINWINDOW_H
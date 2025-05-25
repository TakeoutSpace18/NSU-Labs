#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QInputDialog>
#include <QSqlDatabase>
#include "permissioncontroller.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createMenus();
    createStatusBar();
    setupTabs();
}

MainWindow::~MainWindow()
{
    if (db.isOpen()) {
        db.close();
    }
}

void MainWindow::setDatabase(const QSqlDatabase &database)
{
    db = database;
    
    // Update views with the database connection
    departmentView->setDatabase(db);
    siteView->setDatabase(db);
    customerView->setDatabase(db);
    objectView->setDatabase(db);
    employeeView->setDatabase(db);
    brigadesView->setDatabase(db);
    workView->setDatabase(db);
    materialsView->setDatabase(db);
    machinesView->setDatabase(db);
    
    statusBar()->showMessage(tr("Connected to database"), 3000);
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    
    QAction *exitAction = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
    exitAction->setShortcut(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Exit the application"));
    
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    
    QAction *aboutAction = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAction->setStatusTip(tr("Show the application's About box"));
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::setupTabs()
{
    tabWidget = new QTabWidget();
    
    // Create view instances
    departmentView = new DepartmentView(this);
    siteView = new SiteView(this);
    customerView = new CustomerView(this);
    objectView = new ObjectView(this);
    employeeView = new EmployeeView(this);
    brigadesView = new BrigadesView(this);
    workView = new WorkView(this);
    materialsView = new MaterialsView(this);
    machinesView = new MachinesView(this);
    
    // Add tabs
    tabWidget->addTab(departmentView, tr("Departments"));
    tabWidget->addTab(siteView, tr("Sites"));
    
    // Only add Customer tab if user has permission to view it
    if (PermissionController::instance().canRead("customer")) {
        tabWidget->addTab(customerView, tr("Customers"));
    }
    
    tabWidget->addTab(objectView, tr("Construction Objects"));
    
    // Only add Employee tab if user has permission to view employee data
    if (PermissionController::instance().canRead("employee")) {
        tabWidget->addTab(employeeView, tr("Employees"));
    }
    
    tabWidget->addTab(brigadesView, tr("Brigades"));
    tabWidget->addTab(workView, tr("Work Schedules"));
    tabWidget->addTab(materialsView, tr("Materials"));
    tabWidget->addTab(machinesView, tr("Machines"));
    
    setCentralWidget(tabWidget);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Construction Management System"),
                      tr("This application provides a graphical interface to manage "
                         "the construction company database.\n\n"
                         "Current user role: %1").arg(
                             PermissionController::instance().property("currentRole").toString()));
}

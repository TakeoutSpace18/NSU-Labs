#include <QAction>
#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>

#include "loginwindow.h"
#include "mainwindow.h"
#include "permissioncontroller.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    LoginWindow loginWindow;

    if (loginWindow.exec() != QDialog::Accepted) {
        return 0;  // User cancelled login
    }

    // Set the current user role for permission checks
    PermissionController::instance().setUserRole(loginWindow.getRole());

    MainWindow mainWindow;
    mainWindow.setWindowTitle(QString("Construction Firm DB Management System - %1 [%2]")
                                  .arg(loginWindow.getUsername())
                                  .arg(loginWindow.getRole()));
    mainWindow.resize(1024, 768);
    mainWindow.setDatabase(loginWindow.getDatabase());
    mainWindow.show();

    return app.exec();
}

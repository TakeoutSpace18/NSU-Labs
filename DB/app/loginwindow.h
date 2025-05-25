#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QSqlDatabase>
#include <QSettings>

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

    QString getUsername() const { return username; }
    QString getRole() const { return role; }
    QSqlDatabase getDatabase() const { return db; }

private slots:
    void attemptLogin();
    void saveConnectionSettings();
    void loadConnectionSettings();

private:
    void setupUI();

    QLineEdit *hostnameEdit;
    QLineEdit *portEdit;
    QLineEdit *databaseNameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *cancelButton;
    QComboBox *roleComboBox;
    
    QString username;
    QString role;
    QSqlDatabase db;
    QSettings settings;
};

#endif // LOGINWINDOW_H

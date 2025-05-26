#include "loginwindow.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QVBoxLayout>

LoginWindow::LoginWindow(QWidget *parent)
    : QDialog(parent), settings("ConstructionFirm", "DatabaseFrontend")
{
    setWindowTitle(tr("Login to Construction Database"));
    setupUI();
    loadConnectionSettings();
}

LoginWindow::~LoginWindow()
{}

void LoginWindow::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    // Database connection details
    hostnameEdit = new QLineEdit(this);
    portEdit = new QLineEdit(this);
    databaseNameEdit = new QLineEdit(this);
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);

    // Role selection
    roleComboBox = new QComboBox(this);
    roleComboBox->addItem(tr("Director"), "director");
    roleComboBox->addItem(tr("HR Manager"), "hr_manager");
    roleComboBox->addItem(tr("Project Manager"), "project_manager");

    connect(roleComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        if (roleComboBox->currentData() == "director") {
            passwordEdit->setText("director");
        }
        if (roleComboBox->currentData() == "hr_manager") {
            passwordEdit->setText("hr_manager");
        }
        if (roleComboBox->currentData() == "project_manager") {
            passwordEdit->setText("project_manager");
        }
    });

    roleComboBox->setCurrentIndex(1);

    formLayout->addRow(tr("Hostname:"), hostnameEdit);
    formLayout->addRow(tr("Port:"), portEdit);
    formLayout->addRow(tr("Database:"), databaseNameEdit);
    formLayout->addRow(tr("Role:"), roleComboBox);
    formLayout->addRow(tr("Password:"), passwordEdit);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    loginButton = new QPushButton(tr("Login"), this);
    cancelButton = new QPushButton(tr("Cancel"), this);

    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::attemptLogin);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    // Set focus to first field
    hostnameEdit->setFocus();
}

void LoginWindow::attemptLogin()
{
    if (db.isOpen()) {
        db.close();
    }

    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(hostnameEdit->text());
    db.setPort(portEdit->text().toInt());
    db.setDatabaseName(databaseNameEdit->text());
    db.setUserName(roleComboBox->currentData().toString());
    db.setPassword(passwordEdit->text());

    if (!db.open()) {
        QMessageBox::critical(
            this, tr("Connection Error"),
            tr("Failed to connect to the database:\n%1").arg(db.lastError().text()));
        return;
    }

    // Verify role permissions (this would typically involve checking database roles)
    // For this template, we'll simulate role verification
    QString requestedRole = roleComboBox->currentData().toString();

    role = requestedRole;

    // Save settings for next login
    saveConnectionSettings();

    accept();
}

void LoginWindow::saveConnectionSettings()
{
    settings.setValue("hostname", hostnameEdit->text());
    settings.setValue("port", portEdit->text());
    settings.setValue("database", databaseNameEdit->text());
    settings.setValue("role", roleComboBox->currentIndex());
}

void LoginWindow::loadConnectionSettings()
{
    hostnameEdit->setText(settings.value("hostname", "localhost").toString());
    portEdit->setText(settings.value("port", "5432").toString());
    databaseNameEdit->setText(settings.value("database", "cfdb").toString());
    roleComboBox->setCurrentIndex(settings.value("role", 0).toInt());
}

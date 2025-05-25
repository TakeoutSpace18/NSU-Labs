#ifndef BASEVIEW_H
#define BASEVIEW_H

#include <qsqlquery.h>
#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDataWidgetMapper>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QCheckBox>
#include <QDialog>

class BaseView : public QWidget
{
    Q_OBJECT

public:
    explicit BaseView(const QString &tableName, QWidget *parent = nullptr);
    virtual void setDatabase(const QSqlDatabase &database);

protected:
    QSqlTableModel *model;
    QTableView *tableView;
    QString tableName;
    
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QPushButton *refreshButton;
    
    virtual void setupUi();
    virtual void createModel();
    virtual QDialog* createRecordDialog(const QSqlRecord &record, bool isNew);
    virtual void updatePermissionBasedUI();

    virtual QSqlQuery insertQuery(const QDialog *dialog) const;
    virtual QSqlQuery updateQuery(const QDialog *dialog) const;

protected slots:
    virtual void _addRecord();
    virtual void _editRecord();
    virtual void _deleteRecord();
    virtual void refreshView();

    virtual void addRecord();
    virtual void editRecord();
    virtual void deleteRecord();
};

#endif // BASEVIEW_H

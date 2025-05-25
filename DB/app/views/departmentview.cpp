#include "departmentview.h"

DepartmentView::DepartmentView(QWidget *parent) : BaseView("department", parent)
{}

QDialog *DepartmentView::createRecordDialog(const QSqlRecord &record, bool isNew)
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(isNew ? tr("Add Department") : tr("Edit Department"));

    QFormLayout *formLayout = new QFormLayout(dialog);

    // For department_id
    QLineEdit *idEdit = new QLineEdit(dialog);
    idEdit->setReadOnly(true);
    if (!isNew) {
        idEdit->setText(record.value("department_id").toString());
    }
    else {
        idEdit->setText(tr("(Auto-generated)"));
    }
    idEdit->setObjectName("department_id");
    formLayout->addRow(tr("Department ID:"), idEdit);

    // For name
    QLineEdit *nameEdit = new QLineEdit(dialog);
    if (!isNew) {
        nameEdit->setText(record.value("name").toString());
    }
    nameEdit->setObjectName("name");
    formLayout->addRow(tr("Name:"), nameEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dialog);

    connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

    formLayout->addRow(buttonBox);

    return dialog;
}

QSqlQuery DepartmentView::insertQuery(const QDialog *dialog) const
{
    QLineEdit *nameEdit = dialog->findChild<QLineEdit *>("name");
    QString name = nameEdit->text();

    QSqlQuery query;
    query.prepare("INSERT INTO department (name) VALUES (:name)");
    query.bindValue(":name", name);

    return query;
}

QSqlQuery DepartmentView::updateQuery(const QDialog *dialog) const
{
    QString name = dialog->findChild<QLineEdit *>("name")->text();
    QString id = dialog->findChild<QLineEdit *>("department_id")->text();

    QSqlQuery query;
    query.prepare("UPDATE department SET name = :name WHERE department_id = :id");
    query.bindValue(":name", name);
    query.bindValue(":id", id);

    return query;
}

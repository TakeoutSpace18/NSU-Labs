#include "siteview.h"

#include <QComboBox>
#include <QSqlQuery>
#include <QSqlQueryModel>

SiteView::SiteView(QWidget *parent) : BaseView("site", parent)
{}

QDialog *SiteView::createRecordDialog(const QSqlRecord &record, bool isNew)
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(isNew ? tr("Add Site") : tr("Edit Site"));

    QFormLayout *formLayout = new QFormLayout(dialog);

    // For site_id
    QLineEdit *idEdit = new QLineEdit(dialog);
    idEdit->setReadOnly(true);
    if (!isNew) {
        idEdit->setText(record.value("site_id").toString());
    }
    else {
        idEdit->setText(tr("(Auto-generated)"));
    }
    idEdit->setObjectName("site_id");
    formLayout->addRow(tr("Site ID:"), idEdit);

    // For department_id - use a combobox with department names
    QComboBox *departmentCombo = new QComboBox(dialog);

    // Populate the combobox with department names and IDs
    QSqlQuery query("SELECT department_id, name FROM department ORDER BY name");
    while (query.next()) {
        int deptId = query.value(0).toInt();
        QString deptName = query.value(1).toString();
        departmentCombo->addItem(deptName, deptId);
    }

    // Set current value if editing
    if (!isNew) {
        int currentDeptId = record.value("department_id").toInt();
        for (int i = 0; i < departmentCombo->count(); ++i) {
            if (departmentCombo->itemData(i).toInt() == currentDeptId) {
                departmentCombo->setCurrentIndex(i);
                break;
            }
        }
    }

    departmentCombo->setObjectName("department_id");
    formLayout->addRow(tr("Department:"), departmentCombo);

    // For address
    QLineEdit *addressEdit = new QLineEdit(dialog);
    if (!isNew) {
        addressEdit->setText(record.value("address").toString());
    }
    addressEdit->setObjectName("address");
    formLayout->addRow(tr("Address:"), addressEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dialog);

    connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

    formLayout->addRow(buttonBox);

    return dialog;
}

void SiteView::_addRecord()
{
    QSqlRecord record = model->record();
    QDialog *dialog = createRecordDialog(record, true);

    if (dialog->exec() == QDialog::Accepted) {
        QComboBox *departmentCombo = dialog->findChild<QComboBox *>("department_id");
        QLineEdit *addressEdit = dialog->findChild<QLineEdit *>("address");

        QSqlRecord newRecord = model->record();
        newRecord.setValue("department_id", departmentCombo->currentData().toInt());
        newRecord.setValue("address", addressEdit->text());

        if (model->insertRecord(-1, newRecord)) {
            model->submitAll();
        }
        else {
            QMessageBox::critical(
                this, tr("Error"),
                tr("Failed to add site: %1").arg(model->lastError().text()));
        }
    }

    delete dialog;
    refreshView();
}

void SiteView::_editRecord()
{
    QModelIndex index = tableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::information(this, tr("No Selection"),
                                 tr("Please select a site to edit."));
        return;
    }

    int row = index.row();
    QSqlRecord record = model->record(row);
    QDialog *dialog = createRecordDialog(record, false);

    if (dialog->exec() == QDialog::Accepted) {
        QComboBox *departmentCombo = dialog->findChild<QComboBox *>("department_id");
        QLineEdit *addressEdit = dialog->findChild<QLineEdit *>("address");

        model->setData(model->index(row, model->fieldIndex("department_id")),
                       departmentCombo->currentData().toInt());
        model->setData(model->index(row, model->fieldIndex("address")),
                       addressEdit->text());

        if (!model->submitAll()) {
            QMessageBox::critical(
                this, tr("Error"),
                tr("Failed to update site: %1").arg(model->lastError().text()));
            model->revertAll();
        }
    }

    delete dialog;
    refreshView();
}

QSqlQuery SiteView::insertQuery(const QDialog *dialog) const
{
    QVariant department_id =
        dialog->findChild<QComboBox *>("department_id")->currentData();
    QString address = dialog->findChild<QLineEdit *>("address")->text();

    QSqlQuery query;
    query.prepare(
        "INSERT INTO site (department_id, address) VALUES (:department_id, :address)");
    query.bindValue(":department_id", department_id);
    query.bindValue(":address", address);

    return query;
}

QSqlQuery SiteView::updateQuery(const QDialog *dialog) const
{
    QVariant site_id =
        dialog->findChild<QLineEdit *>("site_id")->text();
    QVariant department_id =
        dialog->findChild<QComboBox *>("department_id")->currentData();
    QString address = dialog->findChild<QLineEdit *>("address")->text();

    QSqlQuery query;
    query.prepare(
        "UPDATE site SET department_id = :department_id, address = :address WHERE "
        "site_id = :site_id");

    query.bindValue(":department_id", department_id);
    query.bindValue(":address", address);
    query.bindValue(":site_id", site_id);

    return query;
}

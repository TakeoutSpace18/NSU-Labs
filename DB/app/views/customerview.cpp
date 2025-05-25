#include "customerview.h"

CustomerView::CustomerView(QWidget *parent) : BaseView("customer", parent)
{}

QDialog *CustomerView::createRecordDialog(const QSqlRecord &record, bool isNew)
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(isNew ? tr("Add Customer") : tr("Edit Customer"));

    QFormLayout *formLayout = new QFormLayout(dialog);

    // For customer_id
    QLineEdit *idEdit = new QLineEdit(dialog);
    idEdit->setReadOnly(true);
    if (!isNew) {
        idEdit->setText(record.value("customer_id").toString());
    }
    else {
        idEdit->setText(tr("(Auto-generated)"));
    }
    idEdit->setObjectName("customer_id");
    formLayout->addRow(tr("Customer ID:"), idEdit);

    // For name
    QLineEdit *nameEdit = new QLineEdit(dialog);
    if (!isNew) {
        nameEdit->setText(record.value("name").toString());
    }
    nameEdit->setObjectName("name");
    formLayout->addRow(tr("Name:"), nameEdit);

    // For address
    QLineEdit *addressEdit = new QLineEdit(dialog);
    if (!isNew && !record.value("address").isNull()) {
        addressEdit->setText(record.value("address").toString());
    }
    addressEdit->setObjectName("address");
    formLayout->addRow(tr("Address:"), addressEdit);

    // For phone
    QLineEdit *phoneEdit = new QLineEdit(dialog);
    if (!isNew && !record.value("phone").isNull()) {
        phoneEdit->setText(record.value("phone").toString());
    }
    phoneEdit->setObjectName("phone");
    phoneEdit->setPlaceholderText("+1234567890");
    formLayout->addRow(tr("Phone:"), phoneEdit);

    // For email
    QLineEdit *emailEdit = new QLineEdit(dialog);
    if (!isNew && !record.value("email").isNull()) {
        emailEdit->setText(record.value("email").toString());
    }
    emailEdit->setObjectName("email");
    emailEdit->setPlaceholderText("example@domain.com");
    formLayout->addRow(tr("Email:"), emailEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dialog);

    connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

    formLayout->addRow(buttonBox);

    return dialog;
}

void CustomerView::_addRecord()
{
    QSqlRecord record = model->record();
    QDialog *dialog = createRecordDialog(record, true);

    if (dialog->exec() == QDialog::Accepted) {
        QLineEdit *nameEdit = dialog->findChild<QLineEdit *>("name");
        QLineEdit *addressEdit = dialog->findChild<QLineEdit *>("address");
        QLineEdit *phoneEdit = dialog->findChild<QLineEdit *>("phone");
        QLineEdit *emailEdit = dialog->findChild<QLineEdit *>("email");

        QSqlRecord newRecord = model->record();
        newRecord.setValue("name", nameEdit->text());

        if (!addressEdit->text().isEmpty()) {
            newRecord.setValue("address", addressEdit->text());
        }

        if (!phoneEdit->text().isEmpty()) {
            newRecord.setValue("phone", phoneEdit->text());
        }

        if (!emailEdit->text().isEmpty()) {
            newRecord.setValue("email", emailEdit->text());
        }

        if (!(model->insertRecord(-1, newRecord) && model->submitAll())) {
            QMessageBox::critical(
                this, tr("Error"),
                tr("Failed to add customer: %1").arg(model->lastError().text()));
        }
    }

    delete dialog;
    refreshView();
}

void CustomerView::_editRecord()
{
    QModelIndex index = tableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::information(this, tr("No Selection"),
                                 tr("Please select a customer to edit."));
        return;
    }

    int row = index.row();
    QSqlRecord record = model->record(row);
    QDialog *dialog = createRecordDialog(record, false);

    if (dialog->exec() == QDialog::Accepted) {
        QLineEdit *nameEdit = dialog->findChild<QLineEdit *>("name");
        QLineEdit *addressEdit = dialog->findChild<QLineEdit *>("address");
        QLineEdit *phoneEdit = dialog->findChild<QLineEdit *>("phone");
        QLineEdit *emailEdit = dialog->findChild<QLineEdit *>("email");

        model->setData(model->index(row, model->fieldIndex("name")), nameEdit->text());

        if (addressEdit->text().isEmpty()) {
            model->setData(model->index(row, model->fieldIndex("address")), QVariant());
        }
        else {
            model->setData(model->index(row, model->fieldIndex("address")),
                           addressEdit->text());
        }

        if (phoneEdit->text().isEmpty()) {
            model->setData(model->index(row, model->fieldIndex("phone")), QVariant());
        }
        else {
            model->setData(model->index(row, model->fieldIndex("phone")),
                           phoneEdit->text());
        }

        if (emailEdit->text().isEmpty()) {
            model->setData(model->index(row, model->fieldIndex("email")), QVariant());
        }
        else {
            model->setData(model->index(row, model->fieldIndex("email")),
                           emailEdit->text());
        }

        if (!model->submitAll()) {
            QMessageBox::critical(
                this, tr("Error"),
                tr("Failed to update customer: %1").arg(model->lastError().text()));
            model->revertAll();
        }
    }

    delete dialog;
    refreshView();
}

QSqlQuery CustomerView::insertQuery(const QDialog *dialog) const
{
    QString name = dialog->findChild<QLineEdit *>("name")->text();
    QString address = dialog->findChild<QLineEdit *>("address")->text();
    QString phone = dialog->findChild<QLineEdit *>("phone")->text();
    QString email = dialog->findChild<QLineEdit *>("email")->text();

    QSqlQuery query;
    query.prepare("INSERT INTO customer (name, address, phone, email) VALUES (:name, :address, :phone, :email)");
    query.bindValue(":name", name);
    query.bindValue(":address", address);
    query.bindValue(":phone", phone);
    query.bindValue(":email", email);

    return query;
}

QSqlQuery CustomerView::updateQuery(const QDialog *dialog) const
{

    QString id = dialog->findChild<QLineEdit *>("customer_id")->text();
    QString name = dialog->findChild<QLineEdit *>("name")->text();
    QString address = dialog->findChild<QLineEdit *>("address")->text();
    QString phone = dialog->findChild<QLineEdit *>("phone")->text();
    QString email = dialog->findChild<QLineEdit *>("email")->text();

    QSqlQuery query;
    query.prepare("UPDATE customer SET name = :name, address = :address, phone = :phone, email = :email WHERE customer_id = :id");
    query.bindValue(":id", id);
    query.bindValue(":name", name);
    query.bindValue(":address", address);
    query.bindValue(":phone", phone);
    query.bindValue(":email", email);

    return query;
}

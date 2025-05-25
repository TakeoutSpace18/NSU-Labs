#include "baseview.h"

#include <qdialog.h>
#include <qglobal.h>
#include <qsqlquery.h>

#include "permissioncontroller.h"

BaseView::BaseView(const QString &tableName, QWidget *parent)
    : QWidget(parent), tableName(tableName)
{
    setupUi();
}

void BaseView::setupUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Create the table view
    tableView = new QTableView(this);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(tableView);

    // Create button layout
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    addButton = new QPushButton(tr("Add"), this);
    editButton = new QPushButton(tr("Edit"), this);
    deleteButton = new QPushButton(tr("Delete"), this);
    refreshButton = new QPushButton(tr("Refresh"), this);

    connect(addButton, &QPushButton::clicked, this, &BaseView::addRecord);
    connect(editButton, &QPushButton::clicked, this, &BaseView::editRecord);
    connect(deleteButton, &QPushButton::clicked, this, &BaseView::deleteRecord);
    connect(refreshButton, &QPushButton::clicked, this, &BaseView::refreshView);

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addStretch();

    layout->addLayout(buttonLayout);

    // Set up permission-based UI
    updatePermissionBasedUI();
}

void BaseView::setDatabase(const QSqlDatabase &database)
{
    createModel();
    refreshView();
}

void BaseView::createModel()
{
    model = new QSqlTableModel(this);
    model->setTable(tableName);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    tableView->setModel(model);
}

QDialog *BaseView::createRecordDialog(const QSqlRecord &record, bool isNew)
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(isNew ? tr("Add Record") : tr("Edit Record"));

    QFormLayout *formLayout = new QFormLayout(dialog);
    QList<QLineEdit *> editors;

    // Create an editor for each field in the record
    for (int i = 0; i < record.count(); i++) {
        if (record.fieldName(i) == tableName + "_id" && isNew) {
            // Skip ID for new records as it's usually auto-generated
            continue;
        }

        QLineEdit *editor = new QLineEdit(dialog);
        if (!isNew) {
            editor->setText(record.value(i).toString());
        }

        formLayout->addRow(record.fieldName(i), editor);
        editors.append(editor);
    }

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dialog);

    connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

    formLayout->addRow(buttonBox);

    return dialog;
}

void BaseView::updatePermissionBasedUI()
{
    // Update UI elements based on user permissions
    addButton->setEnabled(PermissionController::instance().canWrite(tableName));
    editButton->setEnabled(PermissionController::instance().canWrite(tableName));
    deleteButton->setEnabled(PermissionController::instance().canDelete(tableName));

    // If the user doesn't have read permission, disable the entire view
    if (!PermissionController::instance().canRead(tableName)) {
        tableView->setEnabled(false);
        addButton->setEnabled(false);
        editButton->setEnabled(false);
        deleteButton->setEnabled(false);
        refreshButton->setEnabled(false);

        QLabel *noAccessLabel =
            new QLabel(tr("You don't have permission to view this data."), this);
        noAccessLabel->setAlignment(Qt::AlignCenter);
        noAccessLabel->setStyleSheet("color: red; font-weight: bold;");

        QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(this->layout());
        if (layout) {
            layout->insertWidget(0, noAccessLabel);
        }
    }
}

void BaseView::_addRecord()
{
    if (!PermissionController::instance().canWrite(tableName)) {
        QMessageBox::warning(
            this, tr("Permission Denied"),
            tr("You don't have permission to add records to this table."));
        return;
    }

    QSqlRecord record = model->record();
    QDialog *dialog = createRecordDialog(record, true);

    if (dialog->exec() == QDialog::Accepted) {
        // Implementation depends on specific table structure
        // Will be overridden in derived classes
    }

    delete dialog;
    refreshView();
}

void BaseView::_editRecord()
{
    if (!PermissionController::instance().canWrite(tableName)) {
        QMessageBox::warning(
            this, tr("Permission Denied"),
            tr("You don't have permission to edit records in this table."));
        return;
    }

    QModelIndex index = tableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::information(this, tr("No Selection"),
                                 tr("Please select a record to edit."));
        return;
    }

    QSqlRecord record = model->record(index.row());
    QDialog *dialog = createRecordDialog(record, false);

    if (dialog->exec() == QDialog::Accepted) {
        // Implementation depends on specific table structure
        // Will be overridden in derived classes
    }

    delete dialog;
    refreshView();
}

void BaseView::_deleteRecord()
{
    if (!PermissionController::instance().canDelete(tableName)) {
        QMessageBox::warning(
            this, tr("Permission Denied"),
            tr("You don't have permission to delete records from this table."));
        return;
    }

    QModelIndex index = tableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::information(this, tr("No Selection"),
                                 tr("Please select a record to delete."));
        return;
    }

    int row = index.row();
    QString idName = model->headerData(0, Qt::Horizontal).toString();
    QVariant id = model->data(model->index(row, 0));

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Confirm Deletion"), tr("Are you sure you want to delete this record?"),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        model->removeRow(row);
        model->submitAll();
        refreshView();
    }
}

void BaseView::refreshView()
{
    model->select();

    // Set readable column headers if needed
    // This would be implemented in derived classes
}

void BaseView::addRecord()
{
    if (!PermissionController::instance().canWrite(tableName)) {
        QMessageBox::warning(
            this, tr("Permission Denied"),
            tr("You don't have permission to add records to this table."));
        return;
    }

    QSqlRecord record = model->record();
    QDialog *dialog = createRecordDialog(record, true);

    if (dialog->exec() != QDialog::Accepted) {
        delete dialog;
        return;
    }

    QSqlQuery query = insertQuery(dialog);

    if (!query.exec()) {
        QMessageBox::critical(
            this, tr("Error"),
            tr("Failed to add record: %1").arg(query.lastError().text()));
    }

    delete dialog;
    refreshView();
}

void BaseView::editRecord()
{
    if (!PermissionController::instance().canWrite(tableName)) {
        QMessageBox::warning(
            this, tr("Permission Denied"),
            tr("You don't have permission to edit records in this table."));
        return;
    }

    QModelIndex index = tableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::information(this, tr("No Selection"),
                                 tr("Please select a record to edit."));
        return;
    }

    QSqlRecord record = model->record(index.row());
    QDialog *dialog = createRecordDialog(record, false);

    if (dialog->exec() != QDialog::Accepted) {
        delete dialog;
        return;
    }

    QSqlQuery query = updateQuery(dialog);

    if (!query.exec()) {
        QMessageBox::critical(
            this, tr("Error"),
            tr("Failed to edit record: %1").arg(query.lastError().text()));
    }

    delete dialog;
    refreshView();
}

void BaseView::deleteRecord()
{
    if (!PermissionController::instance().canDelete(tableName)) {
        QMessageBox::warning(
            this, tr("Permission Denied"),
            tr("You don't have permission to delete records from this table."));
        return;
    }

    QModelIndex index = tableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::information(this, tr("No Selection"),
                                 tr("Please select a record to delete."));
        return;
    }

    int row = index.row();
    QString idName = model->headerData(0, Qt::Horizontal).toString();
    QVariant id = model->data(model->index(row, 0));

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Confirm Deletion"), tr("Are you sure you want to delete this record?"),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    QSqlQuery query;
    query.prepare(QString("DELETE FROM %1 WHERE %2 = :id").arg(tableName).arg(idName));
    query.bindValue(":id", id);

    if (!query.exec()) {
        QMessageBox::critical(
            this, tr("Error"),
            tr("Failed to delete record: %1").arg(query.lastError().text()));
    }

    refreshView();
}

QSqlQuery BaseView::insertQuery(const QDialog *dialog) const
{
    return QSqlQuery();
}

QSqlQuery BaseView::updateQuery(const QDialog *dialog) const
{
    return QSqlQuery();
}

#include "brigadesview.h"

#include <QComboBox>
#include <QDateEdit>
#include <QSqlQuery>

// Helper class for brigades
class BrigadeView : public BaseView
{
public:
    BrigadeView(QWidget *parent = nullptr) : BaseView("brigade", parent) {}

protected:
    QDialog *createRecordDialog(const QSqlRecord &record, bool isNew) override
    {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle(isNew ? tr("Add Brigade") : tr("Edit Brigade"));

        QFormLayout *formLayout = new QFormLayout(dialog);

        // For brigade_id
        QLineEdit *idEdit = new QLineEdit(dialog);
        idEdit->setReadOnly(true);
        if (!isNew) {
            idEdit->setText(record.value("brigade_id").toString());
        }
        else {
            idEdit->setText(tr("(Auto-generated)"));
        }
        idEdit->setObjectName("brigade_id");
        formLayout->addRow(tr("Brigade ID:"), idEdit);

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

    void _addRecord() override
    {
        QSqlRecord record = model->record();
        QDialog *dialog = createRecordDialog(record, true);

        if (dialog->exec() == QDialog::Accepted) {
            QLineEdit *nameEdit = dialog->findChild<QLineEdit *>("name");

            QSqlRecord newRecord = model->record();
            newRecord.setValue("name", nameEdit->text());

            if (model->insertRecord(-1, newRecord)) {
                model->submitAll();
            }
            else {
                QMessageBox::critical(
                    this, tr("Error"),
                    tr("Failed to add brigade: %1").arg(model->lastError().text()));
            }
        }

        delete dialog;
        refreshView();
    }

    void _editRecord() override
    {
        QModelIndex index = tableView->currentIndex();
        if (!index.isValid()) {
            QMessageBox::information(this, tr("No Selection"),
                                     tr("Please select a brigade to edit."));
            return;
        }

        int row = index.row();
        QSqlRecord record = model->record(row);
        QDialog *dialog = createRecordDialog(record, false);

        if (dialog->exec() == QDialog::Accepted) {
            QLineEdit *nameEdit = dialog->findChild<QLineEdit *>("name");

            model->setData(model->index(row, model->fieldIndex("name")), nameEdit->text());

            if (!model->submitAll()) {
                QMessageBox::critical(
                    this, tr("Error"),
                    tr("Failed to update brigade: %1").arg(model->lastError().text()));
                model->revertAll();
            }
        }

        delete dialog;
        refreshView();
    }
};

// Helper class for brigade workers
class BrigadeWorkerView : public BaseView
{
public:
    BrigadeWorkerView(QWidget *parent = nullptr) : BaseView("brigade_worker", parent) {}

protected:
    QDialog *createRecordDialog(const QSqlRecord &record, bool isNew) override
    {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle(isNew ? tr("Add Brigade Worker") : tr("Edit Brigade Worker"));

        QFormLayout *formLayout = new QFormLayout(dialog);

        // For brigade_worker_id
        QLineEdit *idEdit = new QLineEdit(dialog);
        idEdit->setReadOnly(true);
        if (!isNew) {
            idEdit->setText(record.value("brigade_worker_id").toString());
        }
        else {
            idEdit->setText(tr("(Auto-generated)"));
        }
        idEdit->setObjectName("brigade_worker_id");
        formLayout->addRow(tr("Brigade Worker ID:"), idEdit);

        // For brigade_id
        QComboBox *brigadeCombo = new QComboBox(dialog);
        QSqlQuery brigadeQuery("SELECT brigade_id, name FROM brigade ORDER BY name");
        while (brigadeQuery.next()) {
            int brigadeId = brigadeQuery.value(0).toInt();
            QString brigadeName = brigadeQuery.value(1).toString();
            brigadeCombo->addItem(brigadeName, brigadeId);
        }

        if (!isNew) {
            int currentBrigadeId = record.value("brigade_id").toInt();
            for (int i = 0; i < brigadeCombo->count(); ++i) {
                if (brigadeCombo->itemData(i).toInt() == currentBrigadeId) {
                    brigadeCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
        brigadeCombo->setObjectName("brigade_id");
        formLayout->addRow(tr("Brigade:"), brigadeCombo);

        // For worker_id
        QComboBox *workerCombo = new QComboBox(dialog);
        QSqlQuery workerQuery(
            "SELECT w.employee_id, e.name || ' ' || e.surname AS full_name, wt.type "
            "FROM worker w "
            "JOIN employee e ON w.employee_id = e.employee_id "
            "JOIN worker_type wt ON w.worker_type_id = wt.worker_type_id "
            "ORDER BY e.surname, e.name");
        while (workerQuery.next()) {
            int workerId = workerQuery.value(0).toInt();
            QString workerName = workerQuery.value(1).toString();
            QString workerType = workerQuery.value(2).toString();
            workerCombo->addItem(QString("%1 (%2)").arg(workerName, workerType), workerId);
        }

        if (!isNew) {
            int currentWorkerId = record.value("worker_id").toInt();
            for (int i = 0; i < workerCombo->count(); ++i) {
                if (workerCombo->itemData(i).toInt() == currentWorkerId) {
                    workerCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
        workerCombo->setObjectName("worker_id");
        formLayout->addRow(tr("Worker:"), workerCombo);

        // For start_date
        QDateEdit *startDateEdit = new QDateEdit(dialog);
        startDateEdit->setCalendarPopup(true);
        if (!isNew && !record.value("start_date").isNull()) {
            startDateEdit->setDate(record.value("start_date").toDate());
        }
        else {
            startDateEdit->setDate(QDate::currentDate());
        }
        startDateEdit->setObjectName("start_date");
        formLayout->addRow(tr("Start Date:"), startDateEdit);

        // For end_date
        QDateEdit *endDateEdit = new QDateEdit(dialog);
        endDateEdit->setCalendarPopup(true);
        QCheckBox *endDateCheckBox = new QCheckBox(tr("Set end date"), dialog);
        endDateEdit->setEnabled(false);

        if (!isNew && !record.value("end_date").isNull()) {
            endDateEdit->setDate(record.value("end_date").toDate());
            endDateCheckBox->setChecked(true);
            endDateEdit->setEnabled(true);
        }
        else {
            endDateEdit->setDate(QDate::currentDate().addMonths(6));
        }

        connect(endDateCheckBox, &QCheckBox::toggled, endDateEdit, &QDateEdit::setEnabled);

        endDateEdit->setObjectName("end_date");
        endDateCheckBox->setObjectName("end_date_check");

        QHBoxLayout *endDateLayout = new QHBoxLayout();
        endDateLayout->addWidget(endDateEdit);
        endDateLayout->addWidget(endDateCheckBox);
        formLayout->addRow(tr("End Date:"), endDateLayout);

        QDialogButtonBox *buttonBox = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dialog);

        connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
        connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

        formLayout->addRow(buttonBox);

        return dialog;
    }

    void _addRecord() override
    {
        QSqlRecord record = model->record();
        QDialog *dialog = createRecordDialog(record, true);

        if (dialog->exec() == QDialog::Accepted) {
            QComboBox *brigadeCombo = dialog->findChild<QComboBox *>("brigade_id");
            QComboBox *workerCombo = dialog->findChild<QComboBox *>("worker_id");
            QDateEdit *startDateEdit = dialog->findChild<QDateEdit *>("start_date");
            QDateEdit *endDateEdit = dialog->findChild<QDateEdit *>("end_date");
            QCheckBox *endDateCheckBox = dialog->findChild<QCheckBox *>("end_date_check");

            QSqlRecord newRecord = model->record();
            newRecord.setValue("brigade_id", brigadeCombo->currentData().toInt());
            newRecord.setValue("worker_id", workerCombo->currentData().toInt());
            newRecord.setValue("start_date", startDateEdit->date());

            if (endDateCheckBox->isChecked()) {
                newRecord.setValue("end_date", endDateEdit->date());
            }

            if (model->insertRecord(-1, newRecord)) {
                model->submitAll();
            }
            else {
                QMessageBox::critical(
                    this, tr("Error"),
                    tr("Failed to add brigade worker: %1").arg(model->lastError().text()));
            }
        }

        delete dialog;
        refreshView();
    }

    void _editRecord() override
    {
        QModelIndex index = tableView->currentIndex();
        if (!index.isValid()) {
            QMessageBox::information(this, tr("No Selection"),
                                     tr("Please select a brigade worker record to edit."));
            return;
        }

        int row = index.row();
        QSqlRecord record = model->record(row);
        QDialog *dialog = createRecordDialog(record, false);

        if (dialog->exec() == QDialog::Accepted) {
            QComboBox *brigadeCombo = dialog->findChild<QComboBox *>("brigade_id");
            QComboBox *workerCombo = dialog->findChild<QComboBox *>("worker_id");
            QDateEdit *startDateEdit = dialog->findChild<QDateEdit *>("start_date");
            QDateEdit *endDateEdit = dialog->findChild<QDateEdit *>("end_date");
            QCheckBox *endDateCheckBox = dialog->findChild<QCheckBox *>("end_date_check");

            model->setData(model->index(row, model->fieldIndex("brigade_id")),
                           brigadeCombo->currentData().toInt());
            model->setData(model->index(row, model->fieldIndex("worker_id")),
                           workerCombo->currentData().toInt());
            model->setData(model->index(row, model->fieldIndex("start_date")),
                           startDateEdit->date());

            if (endDateCheckBox->isChecked()) {
                model->setData(model->index(row, model->fieldIndex("end_date")),
                               endDateEdit->date());
            }
            else {
                model->setData(model->index(row, model->fieldIndex("end_date")), QVariant());
            }

            if (!model->submitAll()) {
                QMessageBox::critical(
                    this, tr("Error"),
                    tr("Failed to update brigade worker: %1").arg(model->lastError().text()));
                model->revertAll();
            }
        }

        delete dialog;
        refreshView();
    }
};

// Helper class for brigadiers
class BrigadierView : public BaseView
{
public:
    BrigadierView(QWidget *parent = nullptr) : BaseView("brigadier", parent) {}

protected:
    QDialog *createRecordDialog(const QSqlRecord &record, bool isNew) override
    {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle(isNew ? tr("Add Brigadier") : tr("Edit Brigadier"));

        QFormLayout *formLayout = new QFormLayout(dialog);

        // For brigadier_id
        QLineEdit *idEdit = new QLineEdit(dialog);
        idEdit->setReadOnly(true);
        if (!isNew) {
            idEdit->setText(record.value("brigadier_id").toString());
        }
        else {
            idEdit->setText(tr("(Auto-generated)"));
        }
        idEdit->setObjectName("brigadier_id");
        formLayout->addRow(tr("Brigadier ID:"), idEdit);

        // For brigade_id
        QComboBox *brigadeCombo = new QComboBox(dialog);
        QSqlQuery brigadeQuery("SELECT brigade_id, name FROM brigade ORDER BY name");
        while (brigadeQuery.next()) {
            int brigadeId = brigadeQuery.value(0).toInt();
            QString brigadeName = brigadeQuery.value(1).toString();
            brigadeCombo->addItem(brigadeName, brigadeId);
        }

        if (!isNew) {
            int currentBrigadeId = record.value("brigade_id").toInt();
            for (int i = 0; i < brigadeCombo->count(); ++i) {
                if (brigadeCombo->itemData(i).toInt() == currentBrigadeId) {
                    brigadeCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
        brigadeCombo->setObjectName("brigade_id");
        formLayout->addRow(tr("Brigade:"), brigadeCombo);

        // For worker_id
        QComboBox *workerCombo = new QComboBox(dialog);
        QSqlQuery workerQuery(
            "SELECT w.employee_id, e.name || ' ' || e.surname AS full_name, wt.type "
            "FROM worker w "
            "JOIN employee e ON w.employee_id = e.employee_id "
            "JOIN worker_type wt ON w.worker_type_id = wt.worker_type_id "
            "ORDER BY e.surname, e.name");
        while (workerQuery.next()) {
            int workerId = workerQuery.value(0).toInt();
            QString workerName = workerQuery.value(1).toString();
            QString workerType = workerQuery.value(2).toString();
            workerCombo->addItem(QString("%1 (%2)").arg(workerName, workerType), workerId);
        }

        if (!isNew) {
            int currentWorkerId = record.value("worker_id").toInt();
            for (int i = 0; i < workerCombo->count(); ++i) {
                if (workerCombo->itemData(i).toInt() == currentWorkerId) {
                    workerCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
        workerCombo->setObjectName("worker_id");
        formLayout->addRow(tr("Worker:"), workerCombo);

        // For start_date
        QDateEdit *startDateEdit = new QDateEdit(dialog);
        startDateEdit->setCalendarPopup(true);
        if (!isNew && !record.value("start_date").isNull()) {
            startDateEdit->setDate(record.value("start_date").toDate());
        }
        else {
            startDateEdit->setDate(QDate::currentDate());
        }
        startDateEdit->setObjectName("start_date");
        formLayout->addRow(tr("Start Date:"), startDateEdit);

        // For end_date
        QDateEdit *endDateEdit = new QDateEdit(dialog);
        endDateEdit->setCalendarPopup(true);
        QCheckBox *endDateCheckBox = new QCheckBox(tr("Set end date"), dialog);
        endDateEdit->setEnabled(false);

        if (!isNew && !record.value("end_date").isNull()) {
            endDateEdit->setDate(record.value("end_date").toDate());
            endDateCheckBox->setChecked(true);
            endDateEdit->setEnabled(true);
        }
        else {
            endDateEdit->setDate(QDate::currentDate().addMonths(6));
        }

        connect(endDateCheckBox, &QCheckBox::toggled, endDateEdit, &QDateEdit::setEnabled);

        endDateEdit->setObjectName("end_date");
        endDateCheckBox->setObjectName("end_date_check");

        QHBoxLayout *endDateLayout = new QHBoxLayout();
        endDateLayout->addWidget(endDateEdit);
        endDateLayout->addWidget(endDateCheckBox);
        formLayout->addRow(tr("End Date:"), endDateLayout);

        QDialogButtonBox *buttonBox = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dialog);

        connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
        connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

        formLayout->addRow(buttonBox);

        return dialog;
    }

    void _addRecord() override
    {
        QSqlRecord record = model->record();
        QDialog *dialog = createRecordDialog(record, true);

        if (dialog->exec() == QDialog::Accepted) {
            QComboBox *brigadeCombo = dialog->findChild<QComboBox *>("brigade_id");
            QComboBox *workerCombo = dialog->findChild<QComboBox *>("worker_id");
            QDateEdit *startDateEdit = dialog->findChild<QDateEdit *>("start_date");
            QDateEdit *endDateEdit = dialog->findChild<QDateEdit *>("end_date");
            QCheckBox *endDateCheckBox = dialog->findChild<QCheckBox *>("end_date_check");

            QSqlRecord newRecord = model->record();
            newRecord.setValue("brigade_id", brigadeCombo->currentData().toInt());
            newRecord.setValue("worker_id", workerCombo->currentData().toInt());
            newRecord.setValue("start_date", startDateEdit->date());

            if (endDateCheckBox->isChecked()) {
                newRecord.setValue("end_date", endDateEdit->date());
            }

            if (model->insertRecord(-1, newRecord)) {
                model->submitAll();
            }
            else {
                QMessageBox::critical(
                    this, tr("Error"),
                    tr("Failed to add brigadier: %1").arg(model->lastError().text()));
            }
        }

        delete dialog;
        refreshView();
    }

    void _editRecord() override
    {
        QModelIndex index = tableView->currentIndex();
        if (!index.isValid()) {
            QMessageBox::information(this, tr("No Selection"),
                                     tr("Please select a brigadier record to edit."));
            return;
        }

        int row = index.row();
        QSqlRecord record = model->record(row);
        QDialog *dialog = createRecordDialog(record, false);

        if (dialog->exec() == QDialog::Accepted) {
            QComboBox *brigadeCombo = dialog->findChild<QComboBox *>("brigade_id");
            QComboBox *workerCombo = dialog->findChild<QComboBox *>("worker_id");
            QDateEdit *startDateEdit = dialog->findChild<QDateEdit *>("start_date");
            QDateEdit *endDateEdit = dialog->findChild<QDateEdit *>("end_date");
            QCheckBox *endDateCheckBox = dialog->findChild<QCheckBox *>("end_date_check");

            model->setData(model->index(row, model->fieldIndex("brigade_id")),
                           brigadeCombo->currentData().toInt());
            model->setData(model->index(row, model->fieldIndex("worker_id")),
                           workerCombo->currentData().toInt());
            model->setData(model->index(row, model->fieldIndex("start_date")),
                           startDateEdit->date());

            if (endDateCheckBox->isChecked()) {
                model->setData(model->index(row, model->fieldIndex("end_date")),
                               endDateEdit->date());
            }
            else {
                model->setData(model->index(row, model->fieldIndex("end_date")), QVariant());
            }

            if (!model->submitAll()) {
                QMessageBox::critical(
                    this, tr("Error"),
                    tr("Failed to update brigadier: %1").arg(model->lastError().text()));
                model->revertAll();
            }
        }

        delete dialog;
        refreshView();
    }
};

// Main BrigadesView implementation
BrigadesView::BrigadesView(QWidget *parent) : QWidget(parent)
{
    tabWidget = new QTabWidget(this);

    brigadeView = new BrigadeView(this);
    brigadeWorkerView = new BrigadeWorkerView(this);
    brigadierView = new BrigadierView(this);

    tabWidget->addTab(brigadeView, tr("Brigades"));
    tabWidget->addTab(brigadeWorkerView, tr("Brigade Workers"));
    tabWidget->addTab(brigadierView, tr("Brigadiers"));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(tabWidget);
    setLayout(layout);
}

void BrigadesView::setDatabase(const QSqlDatabase &database)
{
    dynamic_cast<BaseView *>(brigadeView)->setDatabase(database);
    dynamic_cast<BaseView *>(brigadeWorkerView)->setDatabase(database);
    dynamic_cast<BaseView *>(brigadierView)->setDatabase(database);
}

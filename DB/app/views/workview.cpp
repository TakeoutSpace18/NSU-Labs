#include "workview.h"
#include <QSqlQuery>
#include <QComboBox>
#include <QDateEdit>
#include <QTextEdit>

// Helper class for work types
class WorkTypeView : public BaseView
{
public:
    WorkTypeView(QWidget *parent = nullptr) : BaseView("work_type", parent) {}

protected:
    QDialog* createRecordDialog(const QSqlRecord &record, bool isNew) override
    {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle(isNew ? tr("Add Work Type") : tr("Edit Work Type"));
        
        QFormLayout *formLayout = new QFormLayout(dialog);
        
        // For work_type_id
        QLineEdit *idEdit = new QLineEdit(dialog);
        idEdit->setReadOnly(true);
        if (!isNew) {
            idEdit->setText(record.value("work_type_id").toString());
        } else {
            idEdit->setText(tr("(Auto-generated)"));
        }
        idEdit->setObjectName("work_type_id");
        formLayout->addRow(tr("Work Type ID:"), idEdit);
        
        // For name
        QLineEdit *nameEdit = new QLineEdit(dialog);
        if (!isNew) {
            nameEdit->setText(record.value("name").toString());
        }
        nameEdit->setObjectName("name");
        formLayout->addRow(tr("Name:"), nameEdit);
        
        // For description
        QTextEdit *descriptionEdit = new QTextEdit(dialog);
        if (!isNew && !record.value("description").isNull()) {
            descriptionEdit->setText(record.value("description").toString());
        }
        descriptionEdit->setObjectName("description");
        formLayout->addRow(tr("Description:"), descriptionEdit);
        
        QDialogButtonBox *buttonBox = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel, 
            Qt::Horizontal, 
            dialog
        );
        
        connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
        connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
        
        formLayout->addRow(buttonBox);
        
        return dialog;
    }
    
    void addRecord() override
    {
        QSqlRecord record = model->record();
        QDialog *dialog = createRecordDialog(record, true);
        
        if (dialog->exec() == QDialog::Accepted) {
            QLineEdit *nameEdit = dialog->findChild<QLineEdit*>("name");
            QTextEdit *descriptionEdit = dialog->findChild<QTextEdit*>("description");
            
            QSqlRecord newRecord = model->record();
            newRecord.setValue("name", nameEdit->text());
            newRecord.setGenerated(0, false);
            
            if (!descriptionEdit->toPlainText().isEmpty()) {
                newRecord.setValue("description", descriptionEdit->toPlainText());
            }
            
            if (model->insertRecord(-1, newRecord)) {
                model->submitAll();
            } else {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to add work type: %1").arg(model->lastError().text()));
            }
        }
        
        delete dialog;
        refreshView();
    }
    
    void editRecord() override
    {
        QModelIndex index = tableView->currentIndex();
        if (!index.isValid()) {
            QMessageBox::information(this, tr("No Selection"), 
                                  tr("Please select a work type to edit."));
            return;
        }
        
        int row = index.row();
        QSqlRecord record = model->record(row);
        QDialog *dialog = createRecordDialog(record, false);
        
        if (dialog->exec() == QDialog::Accepted) {
            QLineEdit *nameEdit = dialog->findChild<QLineEdit*>("name");
            QTextEdit *descriptionEdit = dialog->findChild<QTextEdit*>("description");
            
            model->setData(model->index(row, model->fieldIndex("name")), nameEdit->text());
            
            if (descriptionEdit->toPlainText().isEmpty()) {
                model->setData(model->index(row, model->fieldIndex("description")), QVariant());
            } else {
                model->setData(model->index(row, model->fieldIndex("description")), descriptionEdit->toPlainText());
            }
            
            if (!model->submitAll()) {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to update work type: %1").arg(model->lastError().text()));
                model->revertAll();
            }
        }
        
        delete dialog;
        refreshView();
    }
};

// Helper class for work schedules
class WorkScheduleView : public BaseView
{
public:
    WorkScheduleView(QWidget *parent = nullptr) : BaseView("work_schedule", parent) {}
    
protected:
    QDialog* createRecordDialog(const QSqlRecord &record, bool isNew) override
    {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle(isNew ? tr("Add Work Schedule") : tr("Edit Work Schedule"));
        
        QFormLayout *formLayout = new QFormLayout(dialog);
        
        // For work_schedule_id
        QLineEdit *idEdit = new QLineEdit(dialog);
        idEdit->setReadOnly(true);
        if (!isNew) {
            idEdit->setText(record.value("work_schedule_id").toString());
        } else {
            idEdit->setText(tr("(Auto-generated)"));
        }
        idEdit->setObjectName("work_schedule_id");
        formLayout->addRow(tr("Schedule ID:"), idEdit);
        
        // For object_id
        QComboBox *objectCombo = new QComboBox(dialog);
        QSqlQuery objectQuery("SELECT object_id, name, type FROM object ORDER BY name");
        while (objectQuery.next()) {
            int objectId = objectQuery.value(0).toInt();
            QString objectName = objectQuery.value(1).toString();
            QString objectType = objectQuery.value(2).toString();
            objectCombo->addItem(QString("%1 (%2)").arg(objectName, objectType), objectId);
        }
        
        if (!isNew) {
            int currentObjectId = record.value("object_id").toInt();
            for (int i = 0; i < objectCombo->count(); ++i) {
                if (objectCombo->itemData(i).toInt() == currentObjectId) {
                    objectCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
        objectCombo->setObjectName("object_id");
        formLayout->addRow(tr("Construction Object:"), objectCombo);
        
        // For work_type_id
        QComboBox *workTypeCombo = new QComboBox(dialog);
        QSqlQuery workTypeQuery("SELECT work_type_id, name FROM work_type ORDER BY name");
        while (workTypeQuery.next()) {
            int workTypeId = workTypeQuery.value(0).toInt();
            QString workTypeName = workTypeQuery.value(1).toString();
            workTypeCombo->addItem(workTypeName, workTypeId);
        }
        
        if (!isNew) {
            int currentWorkTypeId = record.value("work_type_id").toInt();
            for (int i = 0; i < workTypeCombo->count(); ++i) {
                if (workTypeCombo->itemData(i).toInt() == currentWorkTypeId) {
                    workTypeCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
        workTypeCombo->setObjectName("work_type_id");
        formLayout->addRow(tr("Work Type:"), workTypeCombo);
        
        // For brigade_id
        QComboBox *brigadeCombo = new QComboBox(dialog);
        brigadeCombo->addItem(tr("(No Brigade Assigned)"), QVariant());
        QSqlQuery brigadeQuery("SELECT brigade_id, name FROM brigade ORDER BY name");
        while (brigadeQuery.next()) {
            int brigadeId = brigadeQuery.value(0).toInt();
            QString brigadeName = brigadeQuery.value(1).toString();
            brigadeCombo->addItem(brigadeName, brigadeId);
        }
        
        if (!isNew && !record.value("brigade_id").isNull()) {
            int currentBrigadeId = record.value("brigade_id").toInt();
            for (int i = 0; i < brigadeCombo->count(); ++i) {
                if (brigadeCombo->itemData(i).isValid() && brigadeCombo->itemData(i).toInt() == currentBrigadeId) {
                    brigadeCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
        brigadeCombo->setObjectName("brigade_id");
        formLayout->addRow(tr("Brigade:"), brigadeCombo);
        
        // For planned_start_date
        QDateEdit *plannedStartDateEdit = new QDateEdit(dialog);
        plannedStartDateEdit->setCalendarPopup(true);
        if (!isNew && !record.value("planned_start_date").isNull()) {
            plannedStartDateEdit->setDate(record.value("planned_start_date").toDate());
        } else {
            plannedStartDateEdit->setDate(QDate::currentDate());
        }
        plannedStartDateEdit->setObjectName("planned_start_date");
        formLayout->addRow(tr("Planned Start:"), plannedStartDateEdit);
        
        // For planned_end_date
        QDateEdit *plannedEndDateEdit = new QDateEdit(dialog);
        plannedEndDateEdit->setCalendarPopup(true);
        if (!isNew && !record.value("planned_end_date").isNull()) {
            plannedEndDateEdit->setDate(record.value("planned_end_date").toDate());
        } else {
            plannedEndDateEdit->setDate(QDate::currentDate().addDays(14));
        }
        plannedEndDateEdit->setObjectName("planned_end_date");
        formLayout->addRow(tr("Planned End:"), plannedEndDateEdit);
        
        // For actual_start_date
        QDateEdit *actualStartDateEdit = new QDateEdit(dialog);
        actualStartDateEdit->setCalendarPopup(true);
        QCheckBox *actualStartCheckBox = new QCheckBox(tr("Set actual start date"), dialog);
        actualStartDateEdit->setEnabled(false);
        
        if (!isNew && !record.value("actual_start_date").isNull()) {
            actualStartDateEdit->setDate(record.value("actual_start_date").toDate());
            actualStartCheckBox->setChecked(true);
            actualStartDateEdit->setEnabled(true);
        } else {
            actualStartDateEdit->setDate(QDate::currentDate());
        }
        
        connect(actualStartCheckBox, &QCheckBox::toggled, actualStartDateEdit, &QDateEdit::setEnabled);
        
        actualStartDateEdit->setObjectName("actual_start_date");
        actualStartCheckBox->setObjectName("actual_start_check");
        
        QHBoxLayout *actualStartLayout = new QHBoxLayout();
        actualStartLayout->addWidget(actualStartDateEdit);
        actualStartLayout->addWidget(actualStartCheckBox);
        formLayout->addRow(tr("Actual Start:"), actualStartLayout);
        
        // For actual_end_date
        QDateEdit *actualEndDateEdit = new QDateEdit(dialog);
        actualEndDateEdit->setCalendarPopup(true);
        QCheckBox *actualEndCheckBox = new QCheckBox(tr("Set actual end date"), dialog);
        actualEndDateEdit->setEnabled(false);
        
        if (!isNew && !record.value("actual_end_date").isNull()) {
            actualEndDateEdit->setDate(record.value("actual_end_date").toDate());
            actualEndCheckBox->setChecked(true);
            actualEndDateEdit->setEnabled(true);
        } else {
            actualEndDateEdit->setDate(QDate::currentDate().addDays(14));
        }
        
        connect(actualEndCheckBox, &QCheckBox::toggled, actualEndDateEdit, &QDateEdit::setEnabled);
        
        actualEndDateEdit->setObjectName("actual_end_date");
        actualEndCheckBox->setObjectName("actual_end_check");
        
        QHBoxLayout *actualEndLayout = new QHBoxLayout();
        actualEndLayout->addWidget(actualEndDateEdit);
        actualEndLayout->addWidget(actualEndCheckBox);
        formLayout->addRow(tr("Actual End:"), actualEndLayout);
        
        QDialogButtonBox *buttonBox = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel, 
            Qt::Horizontal, 
            dialog
        );
        
        connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
        connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
        
        formLayout->addRow(buttonBox);
        
        return dialog;
    }
    
    void addRecord() override
    {
        QSqlRecord record = model->record();
        QDialog *dialog = createRecordDialog(record, true);
        
        if (dialog->exec() == QDialog::Accepted) {
            QComboBox *objectCombo = dialog->findChild<QComboBox*>("object_id");
            QComboBox *workTypeCombo = dialog->findChild<QComboBox*>("work_type_id");
            QComboBox *brigadeCombo = dialog->findChild<QComboBox*>("brigade_id");
            QDateEdit *plannedStartDateEdit = dialog->findChild<QDateEdit*>("planned_start_date");
            QDateEdit *plannedEndDateEdit = dialog->findChild<QDateEdit*>("planned_end_date");
            QDateEdit *actualStartDateEdit = dialog->findChild<QDateEdit*>("actual_start_date");
            QCheckBox *actualStartCheckBox = dialog->findChild<QCheckBox*>("actual_start_check");
            QDateEdit *actualEndDateEdit = dialog->findChild<QDateEdit*>("actual_end_date");
            QCheckBox *actualEndCheckBox = dialog->findChild<QCheckBox*>("actual_end_check");
            
            QSqlRecord newRecord = model->record();
            newRecord.setGenerated(0, false);
            newRecord.setValue("object_id", objectCombo->currentData().toInt());
            newRecord.setValue("work_type_id", workTypeCombo->currentData().toInt());
            
            if (brigadeCombo->currentData().isValid()) {
                newRecord.setValue("brigade_id", brigadeCombo->currentData().toInt());
            }
            
            newRecord.setValue("planned_start_date", plannedStartDateEdit->date());
            newRecord.setValue("planned_end_date", plannedEndDateEdit->date());
            
            if (actualStartCheckBox->isChecked()) {
                newRecord.setValue("actual_start_date", actualStartDateEdit->date());
            }
            
            if (actualEndCheckBox->isChecked()) {
                newRecord.setValue("actual_end_date", actualEndDateEdit->date());
            }
            
            if (model->insertRecord(-1, newRecord)) {
                model->submitAll();
            } else {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to add work schedule: %1").arg(model->lastError().text()));
            }
        }
        
        delete dialog;
        refreshView();
    }
    
    void editRecord() override
    {
        QModelIndex index = tableView->currentIndex();
        if (!index.isValid()) {
            QMessageBox::information(this, tr("No Selection"), 
                                  tr("Please select a work schedule to edit."));
            return;
        }
        
        int row = index.row();
        QSqlRecord record = model->record(row);
        QDialog *dialog = createRecordDialog(record, false);
        
        if (dialog->exec() == QDialog::Accepted) {
            QComboBox *objectCombo = dialog->findChild<QComboBox*>("object_id");
            QComboBox *workTypeCombo = dialog->findChild<QComboBox*>("work_type_id");
            QComboBox *brigadeCombo = dialog->findChild<QComboBox*>("brigade_id");
            QDateEdit *plannedStartDateEdit = dialog->findChild<QDateEdit*>("planned_start_date");
            QDateEdit *plannedEndDateEdit = dialog->findChild<QDateEdit*>("planned_end_date");
            QDateEdit *actualStartDateEdit = dialog->findChild<QDateEdit*>("actual_start_date");
            QCheckBox *actualStartCheckBox = dialog->findChild<QCheckBox*>("actual_start_check");
            QDateEdit *actualEndDateEdit = dialog->findChild<QDateEdit*>("actual_end_date");
            QCheckBox *actualEndCheckBox = dialog->findChild<QCheckBox*>("actual_end_check");
            
            model->setData(model->index(row, model->fieldIndex("object_id")), objectCombo->currentData().toInt());
            model->setData(model->index(row, model->fieldIndex("work_type_id")), workTypeCombo->currentData().toInt());
            
            if (brigadeCombo->currentData().isValid()) {
                model->setData(model->index(row, model->fieldIndex("brigade_id")), brigadeCombo->currentData());
            } else {
                model->setData(model->index(row, model->fieldIndex("brigade_id")), QVariant());
            }
            
            model->setData(model->index(row, model->fieldIndex("planned_start_date")), plannedStartDateEdit->date());
            model->setData(model->index(row, model->fieldIndex("planned_end_date")), plannedEndDateEdit->date());
            
            if (actualStartCheckBox->isChecked()) {
                model->setData(model->index(row, model->fieldIndex("actual_start_date")), actualStartDateEdit->date());
            } else {
                model->setData(model->index(row, model->fieldIndex("actual_start_date")), QVariant());
            }
            
            if (actualEndCheckBox->isChecked()) {
                model->setData(model->index(row, model->fieldIndex("actual_end_date")), actualEndDateEdit->date());
            } else {
                model->setData(model->index(row, model->fieldIndex("actual_end_date")), QVariant());
            }
            
            if (!model->submitAll()) {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to update work schedule: %1").arg(model->lastError().text()));
                model->revertAll();
            }
        }
        
        delete dialog;
        refreshView();
    }
};

// Main WorkView implementation
WorkView::WorkView(QWidget *parent) : QWidget(parent)
{
    tabWidget = new QTabWidget(this);
    
    workTypeView = new WorkTypeView(this);
    workScheduleView = new WorkScheduleView(this);
    
    tabWidget->addTab(workScheduleView, tr("Work Schedules"));
    tabWidget->addTab(workTypeView, tr("Work Types"));
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(tabWidget);
    setLayout(layout);
}

void WorkView::setDatabase(const QSqlDatabase &database)
{
    dynamic_cast<BaseView*>(workTypeView)->setDatabase(database);
    dynamic_cast<BaseView*>(workScheduleView)->setDatabase(database);
}

#include "machinesview.h"

#include <QSqlQuery>
#include <QComboBox>
#include <QSpinBox>
#include <QDateEdit>
#include <QTextEdit>

// Helper class for machine types
class MachineTypeView : public BaseView
{
public:
    MachineTypeView(QWidget *parent = nullptr) : BaseView("machine_type", parent) {}
    
protected:
    QDialog* createRecordDialog(const QSqlRecord &record, bool isNew) override
    {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle(isNew ? tr("Add Machine Type") : tr("Edit Machine Type"));
        
        QFormLayout *formLayout = new QFormLayout(dialog);
        
        // For machine_type_id
        QLineEdit *idEdit = new QLineEdit(dialog);
        idEdit->setReadOnly(true);
        if (!isNew) {
            idEdit->setText(record.value("machine_type_id").toString());
        } else {
            idEdit->setText(tr("(Auto-generated)"));
        }
        idEdit->setObjectName("machine_type_id");
        formLayout->addRow(tr("Machine Type ID:"), idEdit);
        
        // For type
        QLineEdit *typeEdit = new QLineEdit(dialog);
        if (!isNew) {
            typeEdit->setText(record.value("type").toString());
        }
        typeEdit->setObjectName("type");
        formLayout->addRow(tr("Type:"), typeEdit);
        
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
    
    void _addRecord() override
    {
        QSqlRecord record = model->record();
        QDialog *dialog = createRecordDialog(record, true);
        
        if (dialog->exec() == QDialog::Accepted) {
            QLineEdit *typeEdit = dialog->findChild<QLineEdit*>("type");
            QTextEdit *descriptionEdit = dialog->findChild<QTextEdit*>("description");
            
            QSqlRecord newRecord = model->record();
            newRecord.setValue("type", typeEdit->text());
            
            if (!descriptionEdit->toPlainText().isEmpty()) {
                newRecord.setValue("description", descriptionEdit->toPlainText());
            }
            
            if (model->insertRecord(-1, newRecord)) {
                model->submitAll();
            } else {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to add machine type: %1").arg(model->lastError().text()));
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
                                  tr("Please select a machine type to edit."));
            return;
        }
        
        int row = index.row();
        QSqlRecord record = model->record(row);
        QDialog *dialog = createRecordDialog(record, false);
        
        if (dialog->exec() == QDialog::Accepted) {
            QLineEdit *typeEdit = dialog->findChild<QLineEdit*>("type");
            QTextEdit *descriptionEdit = dialog->findChild<QTextEdit*>("description");
            
            model->setData(model->index(row, model->fieldIndex("type")), typeEdit->text());
            
            if (descriptionEdit->toPlainText().isEmpty()) {
                model->setData(model->index(row, model->fieldIndex("description")), QVariant());
            } else {
                model->setData(model->index(row, model->fieldIndex("description")), descriptionEdit->toPlainText());
            }
            
            if (!model->submitAll()) {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to update machine type: %1").arg(model->lastError().text()));
                model->revertAll();
            }
        }
        
        delete dialog;
        refreshView();
    }
};

// Helper class for machine models
class MachineModelView : public BaseView
{
public:
    MachineModelView(QWidget *parent = nullptr) : BaseView("machine_model", parent) {}
    
protected:
    QDialog* createRecordDialog(const QSqlRecord &record, bool isNew) override
    {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle(isNew ? tr("Add Machine Model") : tr("Edit Machine Model"));
        
        QFormLayout *formLayout = new QFormLayout(dialog);
        
        // For machine_model_id
        QLineEdit *idEdit = new QLineEdit(dialog);
        idEdit->setReadOnly(true);
        if (!isNew) {
            idEdit->setText(record.value("machine_model_id").toString());
        } else {
            idEdit->setText(tr("(Auto-generated)"));
        }
        idEdit->setObjectName("machine_model_id");
        formLayout->addRow(tr("Machine Model ID:"), idEdit);
        
        // For model
        QLineEdit *modelEdit = new QLineEdit(dialog);
        if (!isNew) {
            modelEdit->setText(record.value("model").toString());
        }
        modelEdit->setObjectName("model");
        formLayout->addRow(tr("Model:"), modelEdit);
        
        // For machine_type_id
        QComboBox *machineTypeCombo = new QComboBox(dialog);
        QSqlQuery typeQuery("SELECT machine_type_id, type FROM machine_type ORDER BY type");
        while (typeQuery.next()) {
            int typeId = typeQuery.value(0).toInt();
            QString typeName = typeQuery.value(1).toString();
            machineTypeCombo->addItem(typeName, typeId);
        }
        
        if (!isNew) {
            int currentTypeId = record.value("machine_type_id").toInt();
            for (int i = 0; i < machineTypeCombo->count(); ++i) {
                if (machineTypeCombo->itemData(i).toInt() == currentTypeId) {
                    machineTypeCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
        machineTypeCombo->setObjectName("machine_type_id");
        formLayout->addRow(tr("Machine Type:"), machineTypeCombo);
        
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
    
    void _addRecord() override
    {
        QSqlRecord record = model->record();
        QDialog *dialog = createRecordDialog(record, true);
        
        if (dialog->exec() == QDialog::Accepted) {
            QLineEdit *modelEdit = dialog->findChild<QLineEdit*>("model");
            QComboBox *machineTypeCombo = dialog->findChild<QComboBox*>("machine_type_id");
            
            QSqlRecord newRecord = model->record();
            newRecord.setValue("model", modelEdit->text());
            newRecord.setValue("machine_type_id", machineTypeCombo->currentData().toInt());
            
            if (model->insertRecord(-1, newRecord)) {
                model->submitAll();
            } else {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to add machine model: %1").arg(model->lastError().text()));
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
                                  tr("Please select a machine model to edit."));
            return;
        }
        
        int row = index.row();
        QSqlRecord record = model->record(row);
        QDialog *dialog = createRecordDialog(record, false);
        
        if (dialog->exec() == QDialog::Accepted) {
            QLineEdit *modelEdit = dialog->findChild<QLineEdit*>("model");
            QComboBox *machineTypeCombo = dialog->findChild<QComboBox*>("machine_type_id");
            
            model->setData(model->index(row, model->fieldIndex("model")), modelEdit->text());
            model->setData(model->index(row, model->fieldIndex("machine_type_id")), machineTypeCombo->currentData().toInt());
            
            if (!model->submitAll()) {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to update machine model: %1").arg(model->lastError().text()));
                model->revertAll();
            }
        }
        
        delete dialog;
        refreshView();
    }
};

// Helper class for machines
class MachineView : public BaseView
{
public:
    MachineView(QWidget *parent = nullptr) : BaseView("machine", parent) {}
    
protected:
    QDialog* createRecordDialog(const QSqlRecord &record, bool isNew) override
    {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle(isNew ? tr("Add Machine") : tr("Edit Machine"));
        
        QFormLayout *formLayout = new QFormLayout(dialog);
        
        // For machine_id
        QLineEdit *idEdit = new QLineEdit(dialog);
        idEdit->setReadOnly(true);
        if (!isNew) {
            idEdit->setText(record.value("machine_id").toString());
        } else {
            idEdit->setText(tr("(Auto-generated)"));
        }
        idEdit->setObjectName("machine_id");
        formLayout->addRow(tr("Machine ID:"), idEdit);
        
        // For machine_model_id
        QComboBox *modelCombo = new QComboBox(dialog);
        QSqlQuery modelQuery("SELECT mm.machine_model_id, mm.model, mt.type "
                           "FROM machine_model mm "
                           "JOIN machine_type mt ON mm.machine_type_id = mt.machine_type_id "
                           "ORDER BY mt.type, mm.model");
        while (modelQuery.next()) {
            int modelId = modelQuery.value(0).toInt();
            QString modelName = modelQuery.value(1).toString();
            QString typeName = modelQuery.value(2).toString();
            modelCombo->addItem(QString("%1 (%2)").arg(modelName, typeName), modelId);
        }
        
        if (!isNew) {
            int currentModelId = record.value("machine_model_id").toInt();
            for (int i = 0; i < modelCombo->count(); ++i) {
                if (modelCombo->itemData(i).toInt() == currentModelId) {
                    modelCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
        modelCombo->setObjectName("machine_model_id");
        formLayout->addRow(tr("Machine Model:"), modelCombo);
        
        // For department_id
        QComboBox *deptCombo = new QComboBox(dialog);
        QSqlQuery deptQuery("SELECT department_id, name FROM department ORDER BY name");
        while (deptQuery.next()) {
            int deptId = deptQuery.value(0).toInt();
            QString deptName = deptQuery.value(1).toString();
            deptCombo->addItem(deptName, deptId);
        }
        
        if (!isNew) {
            int currentDeptId = record.value("department_id").toInt();
            for (int i = 0; i < deptCombo->count(); ++i) {
                if (deptCombo->itemData(i).toInt() == currentDeptId) {
                    deptCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
        deptCombo->setObjectName("department_id");
        formLayout->addRow(tr("Department:"), deptCombo);
        
        // For production_year
        QSpinBox *yearSpinBox = new QSpinBox(dialog);
        yearSpinBox->setRange(1950, QDate::currentDate().year());
        
        if (!isNew) {
            yearSpinBox->setValue(record.value("production_year").toInt());
        } else {
            yearSpinBox->setValue(QDate::currentDate().year());
        }
        
        yearSpinBox->setObjectName("production_year");
        formLayout->addRow(tr("Production Year:"), yearSpinBox);
        
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
    
    void _addRecord() override
    {
        QSqlRecord record = model->record();
        QDialog *dialog = createRecordDialog(record, true);
        
        if (dialog->exec() == QDialog::Accepted) {
            QComboBox *modelCombo = dialog->findChild<QComboBox*>("machine_model_id");
            QComboBox *deptCombo = dialog->findChild<QComboBox*>("department_id");
            QSpinBox *yearSpinBox = dialog->findChild<QSpinBox*>("production_year");
            
            QSqlRecord newRecord = model->record();
            newRecord.setValue("machine_model_id", modelCombo->currentData().toInt());
            newRecord.setValue("department_id", deptCombo->currentData().toInt());
            newRecord.setValue("production_year", yearSpinBox->value());
            
            if (model->insertRecord(-1, newRecord)) {
                model->submitAll();
            } else {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to add machine: %1").arg(model->lastError().text()));
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
                                  tr("Please select a machine to edit."));
            return;
        }
        
        int row = index.row();
        QSqlRecord record = model->record(row);
        QDialog *dialog = createRecordDialog(record, false);
        
        if (dialog->exec() == QDialog::Accepted) {
            QComboBox *modelCombo = dialog->findChild<QComboBox*>("machine_model_id");
            QComboBox *deptCombo = dialog->findChild<QComboBox*>("department_id");
            QSpinBox *yearSpinBox = dialog->findChild<QSpinBox*>("production_year");
            
            model->setData(model->index(row, model->fieldIndex("machine_model_id")), modelCombo->currentData().toInt());
            model->setData(model->index(row, model->fieldIndex("department_id")), deptCombo->currentData().toInt());
            model->setData(model->index(row, model->fieldIndex("production_year")), yearSpinBox->value());
            
            if (!model->submitAll()) {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to update machine: %1").arg(model->lastError().text()));
                model->revertAll();
            }
        }
        
        delete dialog;
        refreshView();
    }
};

// Helper class for site machines
class SiteMachineView : public BaseView
{
public:
    SiteMachineView(QWidget *parent = nullptr) : BaseView("site_machine", parent) {}
    
protected:
    QDialog* createRecordDialog(const QSqlRecord &record, bool isNew) override
    {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle(isNew ? tr("Assign Machine to Site") : tr("Edit Site Machine Assignment"));
        
        QFormLayout *formLayout = new QFormLayout(dialog);
        
        // For site_machine_id
        QLineEdit *idEdit = new QLineEdit(dialog);
        idEdit->setReadOnly(true);
        if (!isNew) {
            idEdit->setText(record.value("site_machine_id").toString());
        } else {
            idEdit->setText(tr("(Auto-generated)"));
        }
        idEdit->setObjectName("site_machine_id");
        formLayout->addRow(tr("Assignment ID:"), idEdit);
        
        // For site_id
        QComboBox *siteCombo = new QComboBox(dialog);
        QSqlQuery siteQuery("SELECT s.site_id, s.address, d.name FROM site s "
                          "JOIN department d ON s.department_id = d.department_id "
                          "ORDER BY d.name, s.address");
        while (siteQuery.next()) {
            int siteId = siteQuery.value(0).toInt();
            QString siteAddress = siteQuery.value(1).toString();
            QString deptName = siteQuery.value(2).toString();
            siteCombo->addItem(QString("%1 (%2)").arg(siteAddress, deptName), siteId);
        }
        
        if (!isNew) {
            int currentSiteId = record.value("site_id").toInt();
            for (int i = 0; i < siteCombo->count(); ++i) {
                if (siteCombo->itemData(i).toInt() == currentSiteId) {
                    siteCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
        siteCombo->setObjectName("site_id");
        formLayout->addRow(tr("Site:"), siteCombo);
        
        // For machine_id
        QComboBox *machineCombo = new QComboBox(dialog);
        QSqlQuery machineQuery("SELECT m.machine_id, mm.model, mt.type, m.production_year, d.name "
                             "FROM machine m "
                             "JOIN machine_model mm ON m.machine_model_id = mm.machine_model_id "
                             "JOIN machine_type mt ON mm.machine_type_id = mt.machine_type_id "
                             "JOIN department d ON m.department_id = d.department_id "
                             "ORDER BY mt.type, mm.model");
        while (machineQuery.next()) {
            int machineId = machineQuery.value(0).toInt();
            QString model = machineQuery.value(1).toString();
            QString type = machineQuery.value(2).toString();
            int year = machineQuery.value(3).toInt();
            QString dept = machineQuery.value(4).toString();
            
            machineCombo->addItem(QString("%1 %2 (%3, %4) - %5").arg(type, model, QString::number(year), dept), machineId);
        }
        
        if (!isNew) {
            int currentMachineId = record.value("machine_id").toInt();
            for (int i = 0; i < machineCombo->count(); ++i) {
                if (machineCombo->itemData(i).toInt() == currentMachineId) {
                    machineCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
        machineCombo->setObjectName("machine_id");
        formLayout->addRow(tr("Machine:"), machineCombo);
        
        // For start_date
        QDateEdit *startDateEdit = new QDateEdit(dialog);
        startDateEdit->setCalendarPopup(true);
        if (!isNew && !record.value("start_date").isNull()) {
            startDateEdit->setDate(record.value("start_date").toDate());
        } else {
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
        } else {
            endDateEdit->setDate(QDate::currentDate().addMonths(1));
        }
        
        connect(endDateCheckBox, &QCheckBox::toggled, endDateEdit, &QDateEdit::setEnabled);
        
        endDateEdit->setObjectName("end_date");
        endDateCheckBox->setObjectName("end_date_check");
        
        QHBoxLayout *endDateLayout = new QHBoxLayout();
        endDateLayout->addWidget(endDateEdit);
        endDateLayout->addWidget(endDateCheckBox);
        formLayout->addRow(tr("End Date:"), endDateLayout);
        
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
    
    void _addRecord() override
    {
        QSqlRecord record = model->record();
        QDialog *dialog = createRecordDialog(record, true);
        
        if (dialog->exec() == QDialog::Accepted) {
            QComboBox *siteCombo = dialog->findChild<QComboBox*>("site_id");
            QComboBox *machineCombo = dialog->findChild<QComboBox*>("machine_id");
            QDateEdit *startDateEdit = dialog->findChild<QDateEdit*>("start_date");
            QDateEdit *endDateEdit = dialog->findChild<QDateEdit*>("end_date");
            QCheckBox *endDateCheckBox = dialog->findChild<QCheckBox*>("end_date_check");
            
            QSqlRecord newRecord = model->record();
            newRecord.setValue("site_id", siteCombo->currentData().toInt());
            newRecord.setValue("machine_id", machineCombo->currentData().toInt());
            newRecord.setValue("start_date", startDateEdit->date());
            
            if (endDateCheckBox->isChecked()) {
                newRecord.setValue("end_date", endDateEdit->date());
            }
            
            if (model->insertRecord(-1, newRecord)) {
                model->submitAll();
            } else {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to assign machine to site: %1").arg(model->lastError().text()));
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
                                  tr("Please select a site machine assignment to edit."));
            return;
        }
        
        int row = index.row();
        QSqlRecord record = model->record(row);
        QDialog *dialog = createRecordDialog(record, false);
        
        if (dialog->exec() == QDialog::Accepted) {
            QComboBox *siteCombo = dialog->findChild<QComboBox*>("site_id");
            QComboBox *machineCombo = dialog->findChild<QComboBox*>("machine_id");
            QDateEdit *startDateEdit = dialog->findChild<QDateEdit*>("start_date");
            QDateEdit *endDateEdit = dialog->findChild<QDateEdit*>("end_date");
            QCheckBox *endDateCheckBox = dialog->findChild<QCheckBox*>("end_date_check");
            
            model->setData(model->index(row, model->fieldIndex("site_id")), siteCombo->currentData().toInt());
            model->setData(model->index(row, model->fieldIndex("machine_id")), machineCombo->currentData().toInt());
            model->setData(model->index(row, model->fieldIndex("start_date")), startDateEdit->date());
            
            if (endDateCheckBox->isChecked()) {
                model->setData(model->index(row, model->fieldIndex("end_date")), endDateEdit->date());
            } else {
                model->setData(model->index(row, model->fieldIndex("end_date")), QVariant());
            }
            
            if (!model->submitAll()) {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to update site machine assignment: %1").arg(model->lastError().text()));
                model->revertAll();
            }
        }
        
        delete dialog;
        refreshView();
    }
};

// Main MachinesView implementation
MachinesView::MachinesView(QWidget *parent) : QWidget(parent)
{
    tabWidget = new QTabWidget(this);
    
    machineView = new MachineView(this);
    machineModelView = new MachineModelView(this);
    machineTypeView = new MachineTypeView(this);
    siteMachineView = new SiteMachineView(this);
    
    tabWidget->addTab(machineView, tr("Machines"));
    tabWidget->addTab(siteMachineView, tr("Site Machine Assignments"));
    tabWidget->addTab(machineModelView, tr("Machine Models"));
    tabWidget->addTab(machineTypeView, tr("Machine Types"));
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(tabWidget);
    setLayout(layout);
}

void MachinesView::setDatabase(const QSqlDatabase &database)
{
    dynamic_cast<BaseView*>(machineView)->setDatabase(database);
    dynamic_cast<BaseView*>(machineModelView)->setDatabase(database);
    dynamic_cast<BaseView*>(machineTypeView)->setDatabase(database);
    dynamic_cast<BaseView*>(siteMachineView)->setDatabase(database);
}

#include "employeeview.h"
#include <QSqlQuery>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QTextEdit>

// Helper class for general employee view
class EmployeeGeneralView : public BaseView
{
public:
    EmployeeGeneralView(QWidget *parent = nullptr) : BaseView("employee", parent) {}
    
protected:
    QDialog* createRecordDialog(const QSqlRecord &record, bool isNew) override
    {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle(isNew ? tr("Add Employee") : tr("Edit Employee"));
        
        QFormLayout *formLayout = new QFormLayout(dialog);
        
        // For employee_id
        QLineEdit *idEdit = new QLineEdit(dialog);
        idEdit->setReadOnly(true);
        if (!isNew) {
            idEdit->setText(record.value("employee_id").toString());
        } else {
            idEdit->setText(tr("(Auto-generated)"));
        }
        idEdit->setObjectName("employee_id");
        formLayout->addRow(tr("Employee ID:"), idEdit);
        
        // For name
        QLineEdit *nameEdit = new QLineEdit(dialog);
        if (!isNew) {
            nameEdit->setText(record.value("name").toString());
        }
        nameEdit->setObjectName("name");
        formLayout->addRow(tr("First Name:"), nameEdit);
        
        // For surname
        QLineEdit *surnameEdit = new QLineEdit(dialog);
        if (!isNew) {
            surnameEdit->setText(record.value("surname").toString());
        }
        surnameEdit->setObjectName("surname");
        formLayout->addRow(tr("Last Name:"), surnameEdit);
        
        // For middlename
        QLineEdit *middlenameEdit = new QLineEdit(dialog);
        if (!isNew && !record.value("middlename").isNull()) {
            middlenameEdit->setText(record.value("middlename").toString());
        }
        middlenameEdit->setObjectName("middlename");
        formLayout->addRow(tr("Middle Name:"), middlenameEdit);
        
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
        
        // For salary
        QDoubleSpinBox *salarySpinBox = new QDoubleSpinBox(dialog);
        salarySpinBox->setRange(0, 9999999.99);
        salarySpinBox->setDecimals(2);
        salarySpinBox->setPrefix("$ ");
        
        if (!isNew && !record.value("salary").isNull()) {
            QString salaryStr = record.value("salary").toString();
            // Extract numeric value from MONEY type (e.g., "$100.00" -> 100.00)
            salaryStr.remove('$');
            salarySpinBox->setValue(salaryStr.toDouble());
        }
        
        salarySpinBox->setObjectName("salary");
        formLayout->addRow(tr("Salary:"), salarySpinBox);
        
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
            QLineEdit *surnameEdit = dialog->findChild<QLineEdit*>("surname");
            QLineEdit *middlenameEdit = dialog->findChild<QLineEdit*>("middlename");
            QLineEdit *phoneEdit = dialog->findChild<QLineEdit*>("phone");
            QLineEdit *emailEdit = dialog->findChild<QLineEdit*>("email");
            QDoubleSpinBox *salarySpinBox = dialog->findChild<QDoubleSpinBox*>("salary");
            
            QSqlRecord newRecord = model->record();
            newRecord.setGenerated(0, false);
            newRecord.setValue("name", nameEdit->text());
            newRecord.setValue("surname", surnameEdit->text());
            
            if (!middlenameEdit->text().isEmpty()) {
                newRecord.setValue("middlename", middlenameEdit->text());
            }
            
            if (!phoneEdit->text().isEmpty()) {
                newRecord.setValue("phone", phoneEdit->text());
            }
            
            if (!emailEdit->text().isEmpty()) {
                newRecord.setValue("email", emailEdit->text());
            }
            
            newRecord.setValue("salary", QString("$%1").arg(salarySpinBox->value(), 0, 'f', 2));
            
            if (model->insertRecord(-1, newRecord)) {
                model->submitAll();
            } else {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to add employee: %1").arg(model->lastError().text()));
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
                                  tr("Please select an employee to edit."));
            return;
        }
        
        int row = index.row();
        QSqlRecord record = model->record(row);
        QDialog *dialog = createRecordDialog(record, false);
        
        if (dialog->exec() == QDialog::Accepted) {
            QLineEdit *nameEdit = dialog->findChild<QLineEdit*>("name");
            QLineEdit *surnameEdit = dialog->findChild<QLineEdit*>("surname");
            QLineEdit *middlenameEdit = dialog->findChild<QLineEdit*>("middlename");
            QLineEdit *phoneEdit = dialog->findChild<QLineEdit*>("phone");
            QLineEdit *emailEdit = dialog->findChild<QLineEdit*>("email");
            QDoubleSpinBox *salarySpinBox = dialog->findChild<QDoubleSpinBox*>("salary");
            
            model->setData(model->index(row, model->fieldIndex("name")), nameEdit->text());
            model->setData(model->index(row, model->fieldIndex("surname")), surnameEdit->text());
            
            if (middlenameEdit->text().isEmpty()) {
                model->setData(model->index(row, model->fieldIndex("middlename")), QVariant());
            } else {
                model->setData(model->index(row, model->fieldIndex("middlename")), middlenameEdit->text());
            }
            
            if (phoneEdit->text().isEmpty()) {
                model->setData(model->index(row, model->fieldIndex("phone")), QVariant());
            } else {
                model->setData(model->index(row, model->fieldIndex("phone")), phoneEdit->text());
            }
            
            if (emailEdit->text().isEmpty()) {
                model->setData(model->index(row, model->fieldIndex("email")), QVariant());
            } else {
                model->setData(model->index(row, model->fieldIndex("email")), emailEdit->text());
            }
            
            model->setData(model->index(row, model->fieldIndex("salary")), QString("$%1").arg(salarySpinBox->value(), 0, 'f', 2));
            
            if (!model->submitAll()) {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to update employee: %1").arg(model->lastError().text()));
                model->revertAll();
            }
        }
        
        delete dialog;
        refreshView();
    }
};

// Helper class for worker types
class WorkerTypeView : public BaseView
{
public:
    WorkerTypeView(QWidget *parent = nullptr) : BaseView("worker_type", parent) {}
    
protected:
    QDialog* createRecordDialog(const QSqlRecord &record, bool isNew) override
    {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle(isNew ? tr("Add Worker Type") : tr("Edit Worker Type"));
        
        QFormLayout *formLayout = new QFormLayout(dialog);
        
        // For worker_type_id
        QLineEdit *idEdit = new QLineEdit(dialog);
        idEdit->setReadOnly(true);
        if (!isNew) {
            idEdit->setText(record.value("worker_type_id").toString());
        } else {
            idEdit->setText(tr("(Auto-generated)"));
        }
        idEdit->setObjectName("worker_type_id");
        formLayout->addRow(tr("Worker Type ID:"), idEdit);
        
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
    
    void addRecord() override
    {
        QSqlRecord record = model->record();
        QDialog *dialog = createRecordDialog(record, true);
        
        if (dialog->exec() == QDialog::Accepted) {
            QLineEdit *typeEdit = dialog->findChild<QLineEdit*>("type");
            QTextEdit *descriptionEdit = dialog->findChild<QTextEdit*>("description");
            
            QSqlRecord newRecord = model->record();
            newRecord.setGenerated(0, false);
            newRecord.setValue("type", typeEdit->text());
            
            if (!descriptionEdit->toPlainText().isEmpty()) {
                newRecord.setValue("description", descriptionEdit->toPlainText());
            }
            
            if (model->insertRecord(-1, newRecord)) {
                model->submitAll();
            } else {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to add worker type: %1").arg(model->lastError().text()));
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
                                  tr("Please select a worker type to edit."));
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
                                    tr("Failed to update worker type: %1").arg(model->lastError().text()));
                model->revertAll();
            }
        }
        
        delete dialog;
        refreshView();
    }
};

// Helper class for specialist types
class SpecialistTypeView : public BaseView
{
public:
    SpecialistTypeView(QWidget *parent = nullptr) : BaseView("specialist_type", parent) {}
    
protected:
    QDialog* createRecordDialog(const QSqlRecord &record, bool isNew) override
    {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle(isNew ? tr("Add Specialist Type") : tr("Edit Specialist Type"));
        
        QFormLayout *formLayout = new QFormLayout(dialog);
        
        // For specialist_type_id
        QLineEdit *idEdit = new QLineEdit(dialog);
        idEdit->setReadOnly(true);
        if (!isNew) {
            idEdit->setText(record.value("specialist_type_id").toString());
        } else {
            idEdit->setText(tr("(Auto-generated)"));
        }
        idEdit->setObjectName("specialist_type_id");
        formLayout->addRow(tr("Specialist Type ID:"), idEdit);
        
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
    
    void addRecord() override
    {
        QSqlRecord record = model->record();
        QDialog *dialog = createRecordDialog(record, true);
        
        if (dialog->exec() == QDialog::Accepted) {
            QLineEdit *typeEdit = dialog->findChild<QLineEdit*>("type");
            QTextEdit *descriptionEdit = dialog->findChild<QTextEdit*>("description");
            
            QSqlRecord newRecord = model->record();
            newRecord.setGenerated(0, false);
            newRecord.setValue("type", typeEdit->text());
            
            if (!descriptionEdit->toPlainText().isEmpty()) {
                newRecord.setValue("description", descriptionEdit->toPlainText());
            }
            
            if (model->insertRecord(-1, newRecord)) {
                model->submitAll();
            } else {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to add specialist type: %1").arg(model->lastError().text()));
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
                                  tr("Please select a specialist type to edit."));
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
                                    tr("Failed to update specialist type: %1").arg(model->lastError().text()));
                model->revertAll();
            }
        }
        
        delete dialog;
        refreshView();
    }
};

// Helper class for workers
class WorkerView : public BaseView
{
public:
    WorkerView(QWidget *parent = nullptr) : BaseView("worker", parent) {}
    
protected:
    QDialog* createRecordDialog(const QSqlRecord &record, bool isNew) override
    {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle(isNew ? tr("Add Worker") : tr("Edit Worker"));
        
        QFormLayout *formLayout = new QFormLayout(dialog);
        
        // For employee_id
        QComboBox *employeeCombo = new QComboBox(dialog);
        
        QSqlQuery query;
        if (isNew) {
            // Query employees who are not yet workers or specialists
            query.exec("SELECT e.employee_id, e.name || ' ' || e.surname AS full_name "
                      "FROM employee e "
                      "LEFT JOIN worker w ON e.employee_id = w.employee_id "
                      "LEFT JOIN specialist s ON e.employee_id = s.employee_id "
                      "WHERE w.employee_id IS NULL AND s.employee_id IS NULL "
                      "ORDER BY e.surname, e.name");
        } else {
            // Query just the specific employee
            query.exec(QString("SELECT e.employee_id, e.name || ' ' || e.surname AS full_name "
                              "FROM employee e WHERE e.employee_id = %1")
                      .arg(record.value("employee_id").toLongLong()));
        }
        
        while (query.next()) {
            int employeeId = query.value(0).toInt();
            QString employeeName = query.value(1).toString();
            employeeCombo->addItem(employeeName, employeeId);
        }
        
        if (!isNew) {
            // For editing, we don't allow changing the employee
            employeeCombo->setEnabled(false);
        }
        
        employeeCombo->setObjectName("employee_id");
        formLayout->addRow(tr("Employee:"), employeeCombo);
        
        // For worker_type_id
        QComboBox *workerTypeCombo = new QComboBox(dialog);
        QSqlQuery workerTypeQuery("SELECT worker_type_id, type FROM worker_type ORDER BY type");
        while (workerTypeQuery.next()) {
            int typeId = workerTypeQuery.value(0).toInt();
            QString typeName = workerTypeQuery.value(1).toString();
            workerTypeCombo->addItem(typeName, typeId);
        }
        
        if (!isNew) {
            int currentTypeId = record.value("worker_type_id").toInt();
            for (int i = 0; i < workerTypeCombo->count(); ++i) {
                if (workerTypeCombo->itemData(i).toInt() == currentTypeId) {
                    workerTypeCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
        workerTypeCombo->setObjectName("worker_type_id");
        formLayout->addRow(tr("Worker Type:"), workerTypeCombo);
        
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
            QComboBox *employeeCombo = dialog->findChild<QComboBox*>("employee_id");
            QComboBox *workerTypeCombo = dialog->findChild<QComboBox*>("worker_type_id");
            
            QSqlRecord newRecord = model->record();
            newRecord.setGenerated(0, false);
            newRecord.setValue("employee_id", employeeCombo->currentData().toInt());
            newRecord.setValue("worker_type_id", workerTypeCombo->currentData().toInt());
            
            if (model->insertRecord(-1, newRecord)) {
                model->submitAll();
            } else {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to add worker: %1").arg(model->lastError().text()));
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
                                  tr("Please select a worker to edit."));
            return;
        }
        
        int row = index.row();
        QSqlRecord record = model->record(row);
        QDialog *dialog = createRecordDialog(record, false);
        
        if (dialog->exec() == QDialog::Accepted) {
            QComboBox *workerTypeCombo = dialog->findChild<QComboBox*>("worker_type_id");
            
            model->setData(model->index(row, model->fieldIndex("worker_type_id")), workerTypeCombo->currentData().toInt());
            
            if (!model->submitAll()) {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to update worker: %1").arg(model->lastError().text()));
                model->revertAll();
            }
        }
        
        delete dialog;
        refreshView();
    }
};

// Helper class for specialists
class SpecialistView : public BaseView
{
public:
    SpecialistView(QWidget *parent = nullptr) : BaseView("specialist", parent) {}
    
protected:
    QDialog* createRecordDialog(const QSqlRecord &record, bool isNew) override
    {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle(isNew ? tr("Add Specialist") : tr("Edit Specialist"));
        
        QFormLayout *formLayout = new QFormLayout(dialog);
        
        // For employee_id
        QComboBox *employeeCombo = new QComboBox(dialog);
        
        QSqlQuery query;
        if (isNew) {
            // Query employees who are not yet workers or specialists
            query.exec("SELECT e.employee_id, e.name || ' ' || e.surname AS full_name "
                      "FROM employee e "
                      "LEFT JOIN worker w ON e.employee_id = w.employee_id "
                      "LEFT JOIN specialist s ON e.employee_id = s.employee_id "
                      "WHERE w.employee_id IS NULL AND s.employee_id IS NULL "
                      "ORDER BY e.surname, e.name");
        } else {
            // Query just the specific employee
            query.exec(QString("SELECT e.employee_id, e.name || ' ' || e.surname AS full_name "
                              "FROM employee e WHERE e.employee_id = %1")
                      .arg(record.value("employee_id").toLongLong()));
        }
        
        while (query.next()) {
            int employeeId = query.value(0).toInt();
            QString employeeName = query.value(1).toString();
            employeeCombo->addItem(employeeName, employeeId);
        }
        
        if (!isNew) {
            // For editing, we don't allow changing the employee
            employeeCombo->setEnabled(false);
        }
        
        employeeCombo->setObjectName("employee_id");
        formLayout->addRow(tr("Employee:"), employeeCombo);
        
        // For specialist_type_id
        QComboBox *specialistTypeCombo = new QComboBox(dialog);
        QSqlQuery specialistTypeQuery("SELECT specialist_type_id, type FROM specialist_type ORDER BY type");
        while (specialistTypeQuery.next()) {
            int typeId = specialistTypeQuery.value(0).toInt();
            QString typeName = specialistTypeQuery.value(1).toString();
            specialistTypeCombo->addItem(typeName, typeId);
        }
        
        if (!isNew) {
            int currentTypeId = record.value("specialist_type_id").toInt();
            for (int i = 0; i < specialistTypeCombo->count(); ++i) {
                if (specialistTypeCombo->itemData(i).toInt() == currentTypeId) {
                    specialistTypeCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
        specialistTypeCombo->setObjectName("specialist_type_id");
        formLayout->addRow(tr("Specialist Type:"), specialistTypeCombo);
        
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
            QComboBox *employeeCombo = dialog->findChild<QComboBox*>("employee_id");
            QComboBox *specialistTypeCombo = dialog->findChild<QComboBox*>("specialist_type_id");
            
            QSqlRecord newRecord = model->record();
            newRecord.setGenerated(0, false);
            newRecord.setValue("employee_id", employeeCombo->currentData().toInt());
            newRecord.setValue("specialist_type_id", specialistTypeCombo->currentData().toInt());
            
            if (model->insertRecord(-1, newRecord)) {
                model->submitAll();
            } else {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to add specialist: %1").arg(model->lastError().text()));
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
                                  tr("Please select a specialist to edit."));
            return;
        }
        
        int row = index.row();
        QSqlRecord record = model->record(row);
        QDialog *dialog = createRecordDialog(record, false);
        
        if (dialog->exec() == QDialog::Accepted) {
            QComboBox *specialistTypeCombo = dialog->findChild<QComboBox*>("specialist_type_id");
            
            model->setData(model->index(row, model->fieldIndex("specialist_type_id")), specialistTypeCombo->currentData().toInt());
            
            if (!model->submitAll()) {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to update specialist: %1").arg(model->lastError().text()));
                model->revertAll();
            }
        }
        
        delete dialog;
        refreshView();
    }
};

// Helper class for grades
class GradeView : public BaseView
{
public:
    GradeView(QWidget *parent = nullptr) : BaseView("grade", parent) {}
    
protected:
    QDialog* createRecordDialog(const QSqlRecord &record, bool isNew) override
    {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle(isNew ? tr("Add Grade") : tr("Edit Grade"));
        
        QFormLayout *formLayout = new QFormLayout(dialog);
        
        // For grade_id
        QLineEdit *idEdit = new QLineEdit(dialog);
        idEdit->setReadOnly(true);
        if (!isNew) {
            idEdit->setText(record.value("grade_id").toString());
        } else {
            idEdit->setText(tr("(Auto-generated)"));
        }
        idEdit->setObjectName("grade_id");
        formLayout->addRow(tr("Grade ID:"), idEdit);
        
        // For grade name
        QLineEdit *gradeEdit = new QLineEdit(dialog);
        if (!isNew) {
            gradeEdit->setText(record.value("grade").toString());
        }
        gradeEdit->setObjectName("grade");
        formLayout->addRow(tr("Grade:"), gradeEdit);
        
        // For bonus_payment
        QDoubleSpinBox *bonusSpinBox = new QDoubleSpinBox(dialog);
        bonusSpinBox->setRange(0, 999999.99);
        bonusSpinBox->setDecimals(2);
        bonusSpinBox->setPrefix("$ ");
        
        if (!isNew && !record.value("bonus_payment").isNull()) {
            QString bonusStr = record.value("bonus_payment").toString();
            // Extract numeric value from MONEY type (e.g., "$100.00" -> 100.00)
            bonusStr.remove('$');
            bonusSpinBox->setValue(bonusStr.toDouble());
        }
        
        bonusSpinBox->setObjectName("bonus_payment");
        formLayout->addRow(tr("Bonus Payment:"), bonusSpinBox);
        
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
            QLineEdit *gradeEdit = dialog->findChild<QLineEdit*>("grade");
            QDoubleSpinBox *bonusSpinBox = dialog->findChild<QDoubleSpinBox*>("bonus_payment");
            
            QSqlRecord newRecord = model->record();
            newRecord.setGenerated(0, false);
            newRecord.setValue("grade", gradeEdit->text());
            newRecord.setValue("bonus_payment", QString("$%1").arg(bonusSpinBox->value(), 0, 'f', 2));
            
            if (model->insertRecord(-1, newRecord)) {
                model->submitAll();
            } else {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to add grade: %1").arg(model->lastError().text()));
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
                                  tr("Please select a grade to edit."));
            return;
        }
        
        int row = index.row();
        QSqlRecord record = model->record(row);
        QDialog *dialog = createRecordDialog(record, false);
        
        if (dialog->exec() == QDialog::Accepted) {
            QLineEdit *gradeEdit = dialog->findChild<QLineEdit*>("grade");
            QDoubleSpinBox *bonusSpinBox = dialog->findChild<QDoubleSpinBox*>("bonus_payment");
            
            model->setData(model->index(row, model->fieldIndex("grade")), gradeEdit->text());
            model->setData(model->index(row, model->fieldIndex("bonus_payment")), QString("$%1").arg(bonusSpinBox->value(), 0, 'f', 2));
            
            if (!model->submitAll()) {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to update grade: %1").arg(model->lastError().text()));
                model->revertAll();
            }
        }
        
        delete dialog;
        refreshView();
    }
};

// Main EmployeeView implementation
EmployeeView::EmployeeView(QWidget *parent) : QWidget(parent)
{
    tabWidget = new QTabWidget(this);
    
    employeeGeneralView = new EmployeeGeneralView(this);
    workerView = new WorkerView(this);
    specialistView = new SpecialistView(this);
    gradeView = new GradeView(this);
    workerTypeView = new WorkerTypeView(this);
    specialistTypeView = new SpecialistTypeView(this);
    
    tabWidget->addTab(employeeGeneralView, tr("Employees"));
    tabWidget->addTab(workerView, tr("Workers"));
    tabWidget->addTab(specialistView, tr("Specialists"));
    tabWidget->addTab(gradeView, tr("Grades"));
    tabWidget->addTab(workerTypeView, tr("Worker Types"));
    tabWidget->addTab(specialistTypeView, tr("Specialist Types"));
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(tabWidget);
    setLayout(layout);
}

void EmployeeView::setDatabase(const QSqlDatabase &database)
{
    dynamic_cast<BaseView*>(employeeGeneralView)->setDatabase(database);
    dynamic_cast<BaseView*>(workerView)->setDatabase(database);
    dynamic_cast<BaseView*>(specialistView)->setDatabase(database);
    dynamic_cast<BaseView*>(gradeView)->setDatabase(database);
    dynamic_cast<BaseView*>(workerTypeView)->setDatabase(database);
    dynamic_cast<BaseView*>(specialistTypeView)->setDatabase(database);
}

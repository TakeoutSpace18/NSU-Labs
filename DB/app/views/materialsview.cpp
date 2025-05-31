#include "materialsview.h"

#include <QDoubleSpinBox>
#include <QTextEdit>

class MaterialView : public BaseView
{
public:
    MaterialView(QWidget *parent = nullptr) : BaseView("material", parent) {}
    
protected:
    QDialog* createRecordDialog(const QSqlRecord &record, bool isNew) override
    {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle(isNew ? tr("Add Material") : tr("Edit Material"));
        
        QFormLayout *formLayout = new QFormLayout(dialog);
        
        // For material_id
        QLineEdit *idEdit = new QLineEdit(dialog);
        idEdit->setReadOnly(true);
        if (!isNew) {
            idEdit->setText(record.value("material_id").toString());
        } else {
            idEdit->setText(tr("(Auto-generated)"));
        }
        idEdit->setObjectName("material_id");
        formLayout->addRow(tr("Material ID:"), idEdit);
        
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
        
        // For unit
        QLineEdit *unitEdit = new QLineEdit(dialog);
        if (!isNew) {
            unitEdit->setText(record.value("unit").toString());
        }
        unitEdit->setObjectName("unit");
        unitEdit->setPlaceholderText(tr("e.g., kg, m, m2, pieces"));
        formLayout->addRow(tr("Unit:"), unitEdit);
        
        // For price
        QDoubleSpinBox *priceSpinBox = new QDoubleSpinBox(dialog);
        priceSpinBox->setMinimum(0.0);
        priceSpinBox->setMaximum(999999.99);
        priceSpinBox->setDecimals(2);
        priceSpinBox->setPrefix("$ ");
        
        if (!isNew && !record.value("price").isNull()) {
            QString priceStr = record.value("price").toString();
            // Extract numeric value from MONEY type (e.g., "$100.00" -> 100.00)
            priceStr.remove('$');
            priceSpinBox->setValue(priceStr.toDouble());
        }
        
        priceSpinBox->setObjectName("price");
        formLayout->addRow(tr("Price:"), priceSpinBox);
        
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
            QLineEdit *unitEdit = dialog->findChild<QLineEdit*>("unit");
            QDoubleSpinBox *priceSpinBox = dialog->findChild<QDoubleSpinBox*>("price");
            
            QSqlRecord newRecord = model->record();
            newRecord.setValue("name", nameEdit->text());
            newRecord.setGenerated(0, false);
            
            if (!descriptionEdit->toPlainText().isEmpty()) {
                newRecord.setValue("description", descriptionEdit->toPlainText());
            }
            
            newRecord.setValue("unit", unitEdit->text());
            newRecord.setValue("price", QString("$%1").arg(priceSpinBox->value(), 0, 'f', 2));
            
            if (model->insertRecord(-1, newRecord)) {
                model->submitAll();
            } else {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to add material: %1").arg(model->lastError().text()));
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
                                  tr("Please select a material to edit."));
            return;
        }
        
        int row = index.row();
        QSqlRecord record = model->record(row);
        QDialog *dialog = createRecordDialog(record, false);
        
        if (dialog->exec() == QDialog::Accepted) {
            QLineEdit *nameEdit = dialog->findChild<QLineEdit*>("name");
            QTextEdit *descriptionEdit = dialog->findChild<QTextEdit*>("description");
            QLineEdit *unitEdit = dialog->findChild<QLineEdit*>("unit");
            QDoubleSpinBox *priceSpinBox = dialog->findChild<QDoubleSpinBox*>("price");
            
            model->setData(model->index(row, model->fieldIndex("name")), nameEdit->text());
            
            if (descriptionEdit->toPlainText().isEmpty()) {
                model->setData(model->index(row, model->fieldIndex("description")), QVariant());
            } else {
                model->setData(model->index(row, model->fieldIndex("description")), descriptionEdit->toPlainText());
            }
            
            model->setData(model->index(row, model->fieldIndex("unit")), unitEdit->text());
            model->setData(model->index(row, model->fieldIndex("price")), QString("$%1").arg(priceSpinBox->value(), 0, 'f', 2));
            
            if (!model->submitAll()) {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to update material: %1").arg(model->lastError().text()));
                model->revertAll();
            }
        }
        
        delete dialog;
        refreshView();
    }
};

class MaterialSupplyView : public BaseView
{
public:
    MaterialSupplyView(QWidget *parent = nullptr) : BaseView("material_supply", parent) {}
    
protected:
    QDialog* createRecordDialog(const QSqlRecord &record, bool isNew) override
    {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle(isNew ? tr("Add Material Supply") : tr("Edit Material Supply"));
        
        QFormLayout *formLayout = new QFormLayout(dialog);
        
        // For material_supply_id
        QLineEdit *idEdit = new QLineEdit(dialog);
        idEdit->setReadOnly(true);
        if (!isNew) {
            idEdit->setText(record.value("material_supply_id").toString());
        } else {
            idEdit->setText(tr("(Auto-generated)"));
        }
        idEdit->setObjectName("material_supply_id");
        formLayout->addRow(tr("Supply ID:"), idEdit);
        
        // For material_id
        QComboBox *materialCombo = new QComboBox(dialog);
        QSqlQuery materialQuery("SELECT material_id, name, unit FROM material ORDER BY name");
        while (materialQuery.next()) {
            int materialId = materialQuery.value(0).toInt();
            QString materialName = materialQuery.value(1).toString();
            QString unit = materialQuery.value(2).toString();
            materialCombo->addItem(QString("%1 (%2)").arg(materialName, unit), materialId);
        }
        
        if (!isNew) {
            int currentMaterialId = record.value("material_id").toInt();
            for (int i = 0; i < materialCombo->count(); ++i) {
                if (materialCombo->itemData(i).toInt() == currentMaterialId) {
                    materialCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
        materialCombo->setObjectName("material_id");
        formLayout->addRow(tr("Material:"), materialCombo);
        
        // For work_schedule_id
        QComboBox *workScheduleCombo = new QComboBox(dialog);
        QSqlQuery workQuery("SELECT ws.work_schedule_id, wt.name, o.name FROM work_schedule ws "
                           "JOIN work_type wt ON ws.work_type_id = wt.work_type_id "
                           "JOIN object o ON ws.object_id = o.object_id "
                           "ORDER BY o.name, wt.name");
        while (workQuery.next()) {
            int workId = workQuery.value(0).toInt();
            QString workName = workQuery.value(1).toString();
            QString objectName = workQuery.value(2).toString();
            workScheduleCombo->addItem(QString("%1 on %2").arg(workName, objectName), workId);
        }
        
        if (!isNew) {
            int currentWorkId = record.value("work_schedule_id").toInt();
            for (int i = 0; i < workScheduleCombo->count(); ++i) {
                if (workScheduleCombo->itemData(i).toInt() == currentWorkId) {
                    workScheduleCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
        workScheduleCombo->setObjectName("work_schedule_id");
        formLayout->addRow(tr("Work Schedule:"), workScheduleCombo);
        
        // For planned_quantity
        QDoubleSpinBox *plannedQuantitySpinBox = new QDoubleSpinBox(dialog);
        plannedQuantitySpinBox->setMinimum(0.01);
        plannedQuantitySpinBox->setMaximum(999999.99);
        plannedQuantitySpinBox->setDecimals(2);
        
        if (!isNew) {
            plannedQuantitySpinBox->setValue(record.value("planned_quantity").toDouble());
        }
        
        plannedQuantitySpinBox->setObjectName("planned_quantity");
        formLayout->addRow(tr("Planned Quantity:"), plannedQuantitySpinBox);
        
        // For actual_quantity
        QDoubleSpinBox *actualQuantitySpinBox = new QDoubleSpinBox(dialog);
        actualQuantitySpinBox->setMinimum(0.01);
        actualQuantitySpinBox->setMaximum(999999.99);
        actualQuantitySpinBox->setDecimals(2);
        QCheckBox *actualQuantityCheckBox = new QCheckBox(tr("Set actual quantity"), dialog);
        actualQuantitySpinBox->setEnabled(false);
        
        if (!isNew && !record.value("actual_quantity").isNull()) {
            actualQuantitySpinBox->setValue(record.value("actual_quantity").toDouble());
            actualQuantityCheckBox->setChecked(true);
            actualQuantitySpinBox->setEnabled(true);
        }
        
        connect(actualQuantityCheckBox, &QCheckBox::toggled, actualQuantitySpinBox, &QDoubleSpinBox::setEnabled);
        
        actualQuantitySpinBox->setObjectName("actual_quantity");
        actualQuantityCheckBox->setObjectName("actual_quantity_check");
        
        QHBoxLayout *actualQuantityLayout = new QHBoxLayout();
        actualQuantityLayout->addWidget(actualQuantitySpinBox);
        actualQuantityLayout->addWidget(actualQuantityCheckBox);
        formLayout->addRow(tr("Actual Quantity:"), actualQuantityLayout);
        
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
            QComboBox *materialCombo = dialog->findChild<QComboBox*>("material_id");
            QComboBox *workScheduleCombo = dialog->findChild<QComboBox*>("work_schedule_id");
            QDoubleSpinBox *plannedQuantitySpinBox = dialog->findChild<QDoubleSpinBox*>("planned_quantity");
            QDoubleSpinBox *actualQuantitySpinBox = dialog->findChild<QDoubleSpinBox*>("actual_quantity");
            QCheckBox *actualQuantityCheckBox = dialog->findChild<QCheckBox*>("actual_quantity_check");
            
            QSqlRecord newRecord = model->record();
            newRecord.setGenerated(0, false);
            newRecord.setValue("material_id", materialCombo->currentData().toInt());
            newRecord.setValue("work_schedule_id", workScheduleCombo->currentData().toInt());
            newRecord.setValue("planned_quantity", plannedQuantitySpinBox->value());
            
            if (actualQuantityCheckBox->isChecked()) {
                newRecord.setValue("actual_quantity", actualQuantitySpinBox->value());
            }
            
            if (model->insertRecord(-1, newRecord)) {
                model->submitAll();
            } else {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to add material supply: %1").arg(model->lastError().text()));
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
                                  tr("Please select a material supply record to edit."));
            return;
        }
        
        int row = index.row();
        QSqlRecord record = model->record(row);
        QDialog *dialog = createRecordDialog(record, false);
        
        if (dialog->exec() == QDialog::Accepted) {
            QComboBox *materialCombo = dialog->findChild<QComboBox*>("material_id");
            QComboBox *workScheduleCombo = dialog->findChild<QComboBox*>("work_schedule_id");
            QDoubleSpinBox *plannedQuantitySpinBox = dialog->findChild<QDoubleSpinBox*>("planned_quantity");
            QDoubleSpinBox *actualQuantitySpinBox = dialog->findChild<QDoubleSpinBox*>("actual_quantity");
            QCheckBox *actualQuantityCheckBox = dialog->findChild<QCheckBox*>("actual_quantity_check");
            
            model->setData(model->index(row, model->fieldIndex("material_id")), materialCombo->currentData().toInt());
            model->setData(model->index(row, model->fieldIndex("work_schedule_id")), workScheduleCombo->currentData().toInt());
            model->setData(model->index(row, model->fieldIndex("planned_quantity")), plannedQuantitySpinBox->value());
            
            if (actualQuantityCheckBox->isChecked()) {
                model->setData(model->index(row, model->fieldIndex("actual_quantity")), actualQuantitySpinBox->value());
            } else {
                model->setData(model->index(row, model->fieldIndex("actual_quantity")), QVariant());
            }
            
            if (!model->submitAll()) {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to update material supply: %1").arg(model->lastError().text()));
                model->revertAll();
            }
        }
        
        delete dialog;
        refreshView();
    }
};

MaterialsView::MaterialsView(QWidget *parent) : QWidget(parent)
{
    tabWidget = new QTabWidget(this);
    
    materialView = new MaterialView(this);
    materialSupplyView = new MaterialSupplyView(this);
    
    tabWidget->addTab(materialView, tr("Materials"));
    tabWidget->addTab(materialSupplyView, tr("Material Supplies"));
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(tabWidget);
    setLayout(layout);
}

void MaterialsView::setDatabase(const QSqlDatabase &database)
{
    dynamic_cast<BaseView*>(materialView)->setDatabase(database);
    dynamic_cast<BaseView*>(materialSupplyView)->setDatabase(database);
}

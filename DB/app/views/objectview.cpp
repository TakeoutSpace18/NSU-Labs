#include "objectview.h"
#include <QSqlQuery>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QDateEdit>

// Helper class for general objects
class GenericObjectView : public BaseView
{
public:
    GenericObjectView(QWidget *parent = nullptr) : BaseView("object", parent) {}
    
protected:
    QDialog* createRecordDialog(const QSqlRecord &record, bool isNew) override
    {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle(isNew ? tr("Add Construction Object") : tr("Edit Construction Object"));
        
        QFormLayout *formLayout = new QFormLayout(dialog);
        
        // For object_id
        QLineEdit *idEdit = new QLineEdit(dialog);
        idEdit->setReadOnly(true);
        if (!isNew) {
            idEdit->setText(record.value("object_id").toString());
        } else {
            idEdit->setText(tr("(Auto-generated)"));
        }
        idEdit->setObjectName("object_id");
        formLayout->addRow(tr("Object ID:"), idEdit);
        
        // For name
        QLineEdit *nameEdit = new QLineEdit(dialog);
        if (!isNew) {
            nameEdit->setText(record.value("name").toString());
        }
        nameEdit->setObjectName("name");
        formLayout->addRow(tr("Name:"), nameEdit);
        
        // For type
        QComboBox *typeCombo = new QComboBox(dialog);
        typeCombo->addItem("bridge", "bridge");
        typeCombo->addItem("residential", "residential");
        typeCombo->addItem("road", "road");
        typeCombo->addItem("hospital", "hospital");
        typeCombo->addItem("school", "school");
        
        if (!isNew) {
            QString type = record.value("type").toString();
            for (int i = 0; i < typeCombo->count(); ++i) {
                if (typeCombo->itemData(i).toString() == type) {
                    typeCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
        typeCombo->setObjectName("type");
        formLayout->addRow(tr("Type:"), typeCombo);
        
        // For site_id
        QComboBox *siteCombo = new QComboBox(dialog);
        QSqlQuery siteQuery("SELECT s.site_id, s.address, d.name FROM site s JOIN department d ON s.department_id = d.department_id ORDER BY d.name, s.address");
        while (siteQuery.next()) {
            int siteId = siteQuery.value(0).toInt();
            QString address = siteQuery.value(1).toString();
            QString deptName = siteQuery.value(2).toString();
            siteCombo->addItem(QString("%1 (%2)").arg(address, deptName), siteId);
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
        
        // For customer_id
        QComboBox *customerCombo = new QComboBox(dialog);
        QSqlQuery customerQuery("SELECT customer_id, name FROM customer ORDER BY name");
        while (customerQuery.next()) {
            int customerId = customerQuery.value(0).toInt();
            QString customerName = customerQuery.value(1).toString();
            customerCombo->addItem(customerName, customerId);
        }
        
        if (!isNew) {
            int currentCustomerId = record.value("customer_id").toInt();
            for (int i = 0; i < customerCombo->count(); ++i) {
                if (customerCombo->itemData(i).toInt() == currentCustomerId) {
                    customerCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
        customerCombo->setObjectName("customer_id");
        formLayout->addRow(tr("Customer:"), customerCombo);
        
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
            plannedEndDateEdit->setDate(QDate::currentDate().addMonths(6));
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
            actualEndDateEdit->setDate(QDate::currentDate().addMonths(6));
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
    
    void _addRecord() override
    {
        QSqlRecord record = model->record();
        QDialog *dialog = createRecordDialog(record, true);
        
        if (dialog->exec() == QDialog::Accepted) {
            QLineEdit *nameEdit = dialog->findChild<QLineEdit*>("name");
            QComboBox *typeCombo = dialog->findChild<QComboBox*>("type");
            QComboBox *siteCombo = dialog->findChild<QComboBox*>("site_id");
            QComboBox *customerCombo = dialog->findChild<QComboBox*>("customer_id");
            QDateEdit *plannedStartDateEdit = dialog->findChild<QDateEdit*>("planned_start_date");
            QDateEdit *plannedEndDateEdit = dialog->findChild<QDateEdit*>("planned_end_date");
            QDateEdit *actualStartDateEdit = dialog->findChild<QDateEdit*>("actual_start_date");
            QCheckBox *actualStartCheckBox = dialog->findChild<QCheckBox*>("actual_start_check");
            QDateEdit *actualEndDateEdit = dialog->findChild<QDateEdit*>("actual_end_date");
            QCheckBox *actualEndCheckBox = dialog->findChild<QCheckBox*>("actual_end_check");
            
            QSqlRecord newRecord = model->record();
            newRecord.setValue("name", nameEdit->text());
            newRecord.setValue("type", typeCombo->currentData().toString());
            newRecord.setValue("site_id", siteCombo->currentData().toInt());
            newRecord.setValue("customer_id", customerCombo->currentData().toInt());
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
                                    tr("Failed to add object: %1").arg(model->lastError().text()));
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
                                  tr("Please select an object to edit."));
            return;
        }
        
        int row = index.row();
        QSqlRecord record = model->record(row);
        QDialog *dialog = createRecordDialog(record, false);
        
        if (dialog->exec() == QDialog::Accepted) {
            QLineEdit *nameEdit = dialog->findChild<QLineEdit*>("name");
            QComboBox *typeCombo = dialog->findChild<QComboBox*>("type");
            QComboBox *siteCombo = dialog->findChild<QComboBox*>("site_id");
            QComboBox *customerCombo = dialog->findChild<QComboBox*>("customer_id");
            QDateEdit *plannedStartDateEdit = dialog->findChild<QDateEdit*>("planned_start_date");
            QDateEdit *plannedEndDateEdit = dialog->findChild<QDateEdit*>("planned_end_date");
            QDateEdit *actualStartDateEdit = dialog->findChild<QDateEdit*>("actual_start_date");
            QCheckBox *actualStartCheckBox = dialog->findChild<QCheckBox*>("actual_start_check");
            QDateEdit *actualEndDateEdit = dialog->findChild<QDateEdit*>("actual_end_date");
            QCheckBox *actualEndCheckBox = dialog->findChild<QCheckBox*>("actual_end_check");
            
            model->setData(model->index(row, model->fieldIndex("name")), nameEdit->text());
            model->setData(model->index(row, model->fieldIndex("type")), typeCombo->currentData().toString());
            model->setData(model->index(row, model->fieldIndex("site_id")), siteCombo->currentData().toInt());
            model->setData(model->index(row, model->fieldIndex("customer_id")), customerCombo->currentData().toInt());
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
                                    tr("Failed to update object: %1").arg(model->lastError().text()));
                model->revertAll();
            }
        }
        
        delete dialog;
        refreshView();
    }
};

// Object type specific view (e.g., BridgeObjectView)
class BridgeObjectView : public BaseView
{
public:
    BridgeObjectView(QWidget *parent = nullptr) : BaseView("bridge_object", parent) {}

protected:
    QDialog* createRecordDialog(const QSqlRecord &record, bool isNew) override
    {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle(isNew ? tr("Add Bridge Details") : tr("Edit Bridge Details"));
        
        QFormLayout *formLayout = new QFormLayout(dialog);
        
        // For object_id
        QComboBox *objectCombo = new QComboBox(dialog);
        
        QSqlQuery query;
        if (isNew) {
            query.exec("SELECT o.object_id, o.name FROM object o LEFT JOIN bridge_object b ON o.object_id = b.object_id "
                      "WHERE o.type = 'bridge' AND b.object_id IS NULL");
        } else {
            query.exec(QString("SELECT object_id, name FROM object WHERE type = 'bridge' AND object_id = %1")
                      .arg(record.value("object_id").toLongLong()));
        }
        
        while (query.next()) {
            int objectId = query.value(0).toInt();
            QString objectName = query.value(1).toString();
            objectCombo->addItem(objectName, objectId);
        }
        
        if (!isNew) {
            for (int i = 0; i < objectCombo->count(); ++i) {
                if (objectCombo->itemData(i).toLongLong() == record.value("object_id").toLongLong()) {
                    objectCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
        
        objectCombo->setObjectName("object_id");
        objectCombo->setEnabled(isNew);  // Can only select object_id when creating new record
        formLayout->addRow(tr("Bridge Object:"), objectCombo);
        
        // For length_m
        QDoubleSpinBox *lengthSpinBox = new QDoubleSpinBox(dialog);
        lengthSpinBox->setMinimum(0.01);
        lengthSpinBox->setMaximum(9999.99);
        lengthSpinBox->setDecimals(2);
        lengthSpinBox->setSuffix(" m");
        
        if (!isNew) {
            lengthSpinBox->setValue(record.value("length_m").toDouble());
        }
        
        lengthSpinBox->setObjectName("length_m");
        formLayout->addRow(tr("Length:"), lengthSpinBox);
        
        // For width_m
        QDoubleSpinBox *widthSpinBox = new QDoubleSpinBox(dialog);
        widthSpinBox->setMinimum(0.01);
        widthSpinBox->setMaximum(999.99);
        widthSpinBox->setDecimals(2);
        widthSpinBox->setSuffix(" m");
        
        if (!isNew) {
            widthSpinBox->setValue(record.value("width_m").toDouble());
        }
        
        widthSpinBox->setObjectName("width_m");
        formLayout->addRow(tr("Width:"), widthSpinBox);
        
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
            QComboBox *objectCombo = dialog->findChild<QComboBox*>("object_id");
            QDoubleSpinBox *lengthSpinBox = dialog->findChild<QDoubleSpinBox*>("length_m");
            QDoubleSpinBox *widthSpinBox = dialog->findChild<QDoubleSpinBox*>("width_m");
            
            QSqlRecord newRecord = model->record();
            newRecord.setValue("object_id", objectCombo->currentData().toLongLong());
            newRecord.setValue("length_m", lengthSpinBox->value());
            newRecord.setValue("width_m", widthSpinBox->value());
            
            if (model->insertRecord(-1, newRecord)) {
                model->submitAll();
            } else {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to add bridge details: %1").arg(model->lastError().text()));
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
                                  tr("Please select a bridge to edit."));
            return;
        }
        
        int row = index.row();
        QSqlRecord record = model->record(row);
        QDialog *dialog = createRecordDialog(record, false);
        
        if (dialog->exec() == QDialog::Accepted) {
            QDoubleSpinBox *lengthSpinBox = dialog->findChild<QDoubleSpinBox*>("length_m");
            QDoubleSpinBox *widthSpinBox = dialog->findChild<QDoubleSpinBox*>("width_m");
            
            model->setData(model->index(row, model->fieldIndex("length_m")), lengthSpinBox->value());
            model->setData(model->index(row, model->fieldIndex("width_m")), widthSpinBox->value());
            
            if (!model->submitAll()) {
                QMessageBox::critical(this, tr("Error"), 
                                    tr("Failed to update bridge details: %1").arg(model->lastError().text()));
                model->revertAll();
            }
        }
        
        delete dialog;
        refreshView();
    }
};

// Main ObjectView implementation
ObjectView::ObjectView(QWidget *parent) : QWidget(parent)
{
    tabWidget = new QTabWidget(this);
    
    generalObjectView = new GenericObjectView(this);
    bridgeObjectView = new BaseView("bridge_object", this);
    residentialObjectView = new BaseView("residential_object", this);
    roadObjectView = new BaseView("road_object", this);
    hospitalObjectView = new BaseView("hospital_object", this);
    schoolObjectView = new BaseView("school_object", this);
    
    tabWidget->addTab(generalObjectView, tr("General Objects"));
    tabWidget->addTab(bridgeObjectView, tr("Bridges"));
    tabWidget->addTab(residentialObjectView, tr("Residential"));
    tabWidget->addTab(roadObjectView, tr("Roads"));
    tabWidget->addTab(hospitalObjectView, tr("Hospitals"));
    tabWidget->addTab(schoolObjectView, tr("Schools"));
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(tabWidget);
    setLayout(layout);
}

void ObjectView::setDatabase(const QSqlDatabase &database)
{
    dynamic_cast<BaseView*>(generalObjectView)->setDatabase(database);
    dynamic_cast<BaseView*>(bridgeObjectView)->setDatabase(database);
    dynamic_cast<BaseView*>(residentialObjectView)->setDatabase(database);
    dynamic_cast<BaseView*>(roadObjectView)->setDatabase(database);
    dynamic_cast<BaseView*>(hospitalObjectView)->setDatabase(database);
    dynamic_cast<BaseView*>(schoolObjectView)->setDatabase(database);
}

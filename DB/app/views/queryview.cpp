#include "queryview.h"

#include <QApplication>
#include <QRegularExpression>
#include <QScrollArea>
#include <QSplitter>
#include <QSqlError>
#include <QString>
#include <QDebug>

// FilterableQueryView Implementation
FilterableQueryView::FilterableQueryView(const QString &queryText, const QString &title,
                                         QWidget *parent)
    : QWidget(parent), baseQuery(queryText), queryTitle(title), hasParameters(false)
{
    parseQueryParameters();
    setupUI();
    setDefaultParameterValues();
}

void FilterableQueryView::setDatabase()
{
    executeQuery();
}

void FilterableQueryView::parseQueryParameters()
{
    // Define parameter patterns and their types
    QMap<QString, QueryParameter> parameterDefinitions;
    
    // ID parameters (integer)
    parameterDefinitions["site_id"] = {"site_id", "int", 1, 1, 999, {}};
    parameterDefinitions["department_id"] = {"department_id", "int", 1, 1, 999, {}};
    parameterDefinitions["object_id"] = {"object_id", "int", 1, 1, 999, {}};
    parameterDefinitions["brigade_id"] = {"brigade_id", "int", 1, 1, 999, {}};
    parameterDefinitions["machine_id"] = {"machine_id", "int", 1, 1, 999, {}};
    parameterDefinitions["employee_id"] = {"employee_id", "int", 1, 1, 999, {}};
    
    // Work type parameter (combo box)
    QStringList workTypes = {"Excavation", "Foundation", "Framing", "Roofing", "Electrical", "Plumbing", "Finishing"};
    parameterDefinitions["work_type"] = {"work_type", "combo", "Excavation", {}, {}, workTypes};
    
    // Date parameters
    parameterDefinitions["start_date"] = {"start_date", "date", QDate::currentDate().addDays(-30), {}, {}, {}};
    parameterDefinitions["end_date"] = {"end_date", "date", QDate::currentDate(), {}, {}, {}};
    parameterDefinitions["date_filter"] = {"date_filter", "date", QDate::currentDate(), {}, {}, {}};
    
    // Find parameters in the query
    QRegularExpression paramRegex(R"(\b(site_id|department_id|object_id|brigade_id|machine_id|employee_id)\s*=\s*(\d+))");
    QRegularExpressionMatchIterator idMatches = paramRegex.globalMatch(baseQuery);
    
    while (idMatches.hasNext()) {
        QRegularExpressionMatch match = idMatches.next();
        QString paramName = match.captured(1);
        int defaultValue = match.captured(2).toInt();
        
        if (parameterDefinitions.contains(paramName)) {
            QueryParameter param = parameterDefinitions[paramName];
            param.defaultValue = defaultValue;
            parameters[paramName] = param;
            hasParameters = true;
        }
    }
    
    // Check for work_type parameter
    QRegularExpression workTypeRegex(R"(\bwork_type\s*=\s*'([^']*)')");
    QRegularExpressionMatch workTypeMatch = workTypeRegex.match(baseQuery);
    if (workTypeMatch.hasMatch()) {
        QString defaultWorkType = workTypeMatch.captured(1);
        QueryParameter param = parameterDefinitions["work_type"];
        param.defaultValue = defaultWorkType;
        parameters["work_type"] = param;
        hasParameters = true;
    }
    
    // Check for date parameters
    QRegularExpression dateRegex(R"('(\d{4}-\d{2}-\d{2})')");
    QRegularExpressionMatchIterator dateMatches = dateRegex.globalMatch(baseQuery);
    
    QStringList foundDates;
    while (dateMatches.hasNext()) {
        QRegularExpressionMatch match = dateMatches.next();
        foundDates.append(match.captured(1));
    }
    
    if (foundDates.size() >= 1) {
        QueryParameter startParam = parameterDefinitions["start_date"];
        startParam.defaultValue = QDate::fromString(foundDates.first(), "yyyy-MM-dd");
        parameters["start_date"] = startParam;
        hasParameters = true;
        
        if (foundDates.size() >= 2) {
            QueryParameter endParam = parameterDefinitions["end_date"];
            endParam.defaultValue = QDate::fromString(foundDates.last(), "yyyy-MM-dd");
            parameters["end_date"] = endParam;
        }
    }
}

void FilterableQueryView::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Create title label
    QLabel *titleLabel = new QLabel(queryTitle, this);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14px; padding: 5px;");

    // Create splitter for main content
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

    // Left side - Parameters and Filters
    QWidget *leftPanel = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);

    // Control buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    executeButton = new QPushButton(tr("Execute Query"), this);
    resetButton = new QPushButton(tr("Reset"), this);

    buttonLayout->addWidget(executeButton);
    buttonLayout->addWidget(resetButton);
    buttonLayout->addStretch();

    leftLayout->addWidget(titleLabel);
    leftLayout->addLayout(buttonLayout);

    // Parameters section
    if (hasParameters) {
        QGroupBox *parameterGroup = new QGroupBox(tr("Parameters"), this);
        QVBoxLayout *parameterGroupLayout = new QVBoxLayout(parameterGroup);
        
        parameterScrollArea = new QScrollArea(this);
        parameterScrollArea->setWidgetResizable(true);
        parameterScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        parameterScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        
        parameterWidget = new QWidget();
        parameterLayout = new QFormLayout(parameterWidget);
        
        createParameterControls();
        
        parameterScrollArea->setWidget(parameterWidget);
        parameterGroupLayout->addWidget(parameterScrollArea);
        
        leftLayout->addWidget(parameterGroup);
    }
    
    leftLayout->addStretch();

    leftPanel->setMaximumWidth(400);
    leftPanel->setMinimumWidth(300);
    splitter->addWidget(leftPanel);

    // Right side - Table view
    tableView = new QTableView(this);
    model = new QSqlQueryModel(this);
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    tableView->setModel(proxyModel);

    tableView->setSortingEnabled(true);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setAlternatingRowColors(true);
    tableView->horizontalHeader()->setStretchLastSection(true);

    splitter->addWidget(tableView);
    splitter->setSizes({350, 650});

    mainLayout->addWidget(splitter);

    // Connect signals
    connect(executeButton, &QPushButton::clicked, this, &FilterableQueryView::executeQuery);
    connect(resetButton, &QPushButton::clicked, this, &FilterableQueryView::resetParameters);
}

void FilterableQueryView::createParameterControls()
{
    for (auto it = parameters.begin(); it != parameters.end(); ++it) {
        const QString &paramName = it.key();
        const QueryParameter &param = it.value();
        
        QWidget *control = createParameterWidget(param);
        if (control) {
            parameterControls[paramName] = control;
            
            QString labelText = paramName;
            labelText.replace('_', ' ');
            labelText = labelText.left(1).toUpper() + labelText.mid(1);
            
            parameterLayout->addRow(labelText + ":", control);
        }
    }
}

QWidget* FilterableQueryView::createParameterWidget(const QueryParameter &param)
{
    if (param.type == "int") {
        QSpinBox *spinBox = new QSpinBox();
        spinBox->setMinimum(param.minValue.toInt());
        spinBox->setMaximum(param.maxValue.toInt());
        spinBox->setValue(param.defaultValue.toInt());
        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &FilterableQueryView::onParameterChanged);
        return spinBox;
    }
    else if (param.type == "combo") {
        QComboBox *comboBox = new QComboBox();
        comboBox->addItems(param.options);
        comboBox->setCurrentText(param.defaultValue.toString());
        connect(comboBox, QOverload<const QString &>::of(&QComboBox::currentTextChanged), this, &FilterableQueryView::onParameterChanged);
        return comboBox;
    }
    else if (param.type == "date") {
        QDateEdit *dateEdit = new QDateEdit();
        dateEdit->setDate(param.defaultValue.toDate());
        dateEdit->setCalendarPopup(true);
        dateEdit->setDisplayFormat("yyyy-MM-dd");
        connect(dateEdit, &QDateEdit::dateChanged, this, &FilterableQueryView::onParameterChanged);
        return dateEdit;
    }
    else if (param.type == "string") {
        QLineEdit *lineEdit = new QLineEdit();
        lineEdit->setText(param.defaultValue.toString());
        connect(lineEdit, &QLineEdit::textChanged, this, &FilterableQueryView::onParameterChanged);
        return lineEdit;
    }
    
    return nullptr;
}

void FilterableQueryView::onParameterChanged()
{
    // Auto-execute query when parameters change (optional)
    // executeQuery();
}

void FilterableQueryView::resetParameters()
{
    setDefaultParameterValues();
    executeQuery();
}

void FilterableQueryView::setDefaultParameterValues()
{
    for (auto it = parameters.begin(); it != parameters.end(); ++it) {
        const QString &paramName = it.key();
        const QueryParameter &param = it.value();
        
        if (parameterControls.contains(paramName)) {
            QWidget *control = parameterControls[paramName];
            
            if (param.type == "int") {
                QSpinBox *spinBox = qobject_cast<QSpinBox*>(control);
                if (spinBox) {
                    spinBox->setValue(param.defaultValue.toInt());
                }
            }
            else if (param.type == "combo") {
                QComboBox *comboBox = qobject_cast<QComboBox*>(control);
                if (comboBox) {
                    comboBox->setCurrentText(param.defaultValue.toString());
                }
            }
            else if (param.type == "date") {
                QDateEdit *dateEdit = qobject_cast<QDateEdit*>(control);
                if (dateEdit) {
                    dateEdit->setDate(param.defaultValue.toDate());
                }
            }
            else if (param.type == "string") {
                QLineEdit *lineEdit = qobject_cast<QLineEdit*>(control);
                if (lineEdit) {
                    lineEdit->setText(param.defaultValue.toString());
                }
            }
        }
    }
}

QVariant FilterableQueryView::getParameterValue(const QString &paramName)
{
    if (!parameterControls.contains(paramName)) {
        return QVariant();
    }
    
    QWidget *control = parameterControls[paramName];
    const QueryParameter &param = parameters[paramName];
    
    if (param.type == "int") {
        QSpinBox *spinBox = qobject_cast<QSpinBox*>(control);
        return spinBox ? spinBox->value() : QVariant();
    }
    else if (param.type == "combo") {
        QComboBox *comboBox = qobject_cast<QComboBox*>(control);
        return comboBox ? comboBox->currentText() : QVariant();
    }
    else if (param.type == "date") {
        QDateEdit *dateEdit = qobject_cast<QDateEdit*>(control);
        return dateEdit ? dateEdit->date() : QVariant();
    }
    else if (param.type == "string") {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(control);
        return lineEdit ? lineEdit->text() : QVariant();
    }
    
    return QVariant();
}

void FilterableQueryView::executeQuery()
{
    if (!QSqlDatabase::database().isValid() || !QSqlDatabase::database().isOpen()) {
        return;
    }

    QString query = buildParameterizedQuery();

    QSqlQuery sqlQuery(QSqlDatabase::database());
    if (!sqlQuery.exec(query)) {
        QMessageBox::critical(
            this, tr("Query Error"),
            tr("Failed to execute query: %1\n\nQuery: %2").arg(sqlQuery.lastError().text(), query));
        return;
    }

    model->setQuery(qMove(sqlQuery));

    if (model->lastError().isValid()) {
        QMessageBox::critical(
            this, tr("Model Error"),
            tr("Failed to load query results: %1").arg(model->lastError().text()));
        return;
    }
    
    // Auto-resize columns to content
    tableView->resizeColumnsToContents();
}

QString FilterableQueryView::buildParameterizedQuery()
{
    QString query = baseQuery;

    if (!hasParameters) {
        return query;
    }

    // Replace integer parameters
    for (const QString &paramName : {"site_id", "department_id", "object_id", "brigade_id", "machine_id", "employee_id"}) {
        if (parameters.contains(paramName)) {
            QVariant value = getParameterValue(paramName);
            if (value.isValid()) {
                QRegularExpression regex(QString(R"(\b%1\s*=\s*\d+)").arg(paramName));
                query.replace(regex, QString("%1 = %2").arg(paramName, value.toString()));
            }
        }
    }
    
    // Replace work_type parameter
    if (parameters.contains("work_type")) {
        QVariant value = getParameterValue("work_type");
        if (value.isValid()) {
            QRegularExpression regex(R"(\bwork_type\s*=\s*'[^']*')");
            query.replace(regex, QString("work_type = '%1'").arg(value.toString()));
        }
    }
    
    // Replace date parameters
    QStringList dateParams = {"start_date", "end_date", "date_filter"};
    QRegularExpression dateRegex(R"('(\d{4}-\d{2}-\d{2})')");
    QRegularExpressionMatchIterator matches = dateRegex.globalMatch(query);
    
    QStringList dateMatches;
    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        dateMatches.append(match.captured(0)); // Full match including quotes
    }
    
    // Replace first date with start_date if available
    if (dateMatches.size() >= 1 && parameters.contains("start_date")) {
        QVariant startDate = getParameterValue("start_date");
        if (startDate.isValid()) {
            QString newDate = QString("'%1'").arg(startDate.toDate().toString("yyyy-MM-dd"));
            query.replace(dateMatches[0], newDate);
        }
    }
    
    // Replace second date with end_date if available
    if (dateMatches.size() >= 2 && parameters.contains("end_date")) {
        QVariant endDate = getParameterValue("end_date");
        if (endDate.isValid()) {
            QString newDate = QString("'%1'").arg(endDate.toDate().toString("yyyy-MM-dd"));
            query.replace(dateMatches[1], newDate);
        }
    }

    return query;
}

// QueryView Implementation
QueryView::QueryView(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    tabWidget = new QTabWidget(this);
    layout->addWidget(tabWidget);

    setupQueries();
}

void QueryView::setupQueries()
{
    addQueryTab(tr("1. Departments & Supervisors"),
                "SELECT department_id, department_name, employee_id, name, surname, "
                "middlename, start_date "
                "FROM v_departments_supervisors WHERE end_date is NULL");

    addQueryTab(tr("1. Sites & Supervisors"),
                "SELECT site_id, address AS site_address, department_name, employee_id, "
                "name, surname, middlename, start_date "
                "FROM v_sites_supervisors WHERE end_date IS NULL");

    addQueryTab(tr("2. Site Specialists"),
                "SELECT employee_id, name, surname, middlename, position, "
                "specialist_type, specialist_description, site_assignment_start_date "
                "FROM v_site_department_specialists WHERE site_id = 9 AND "
                "site_assignment_end_date is NULL");

    addQueryTab(
        tr("3. Department Objects"),
        "SELECT object_id, object_name, object_type, site_address, department_name, "
        "customer_name, "
        "planned_start_date, planned_end_date, actual_start_date, actual_end_date "
        "FROM v_department_site_objects WHERE department_id = 20");

    addQueryTab(tr("4. Object Brigades"),
                "SELECT brigade_id, brigade_name, employee_id, employee_name, "
                "employee_surname, employee_middlename, "
                "worker_position, worker_type, brigade_assignment_start_date, "
                "brigade_assignment_end_date "
                "FROM v_object_brigades WHERE object_id = 1 ORDER BY brigade_name");

    addQueryTab(tr("5. Department Machines"),
                "SELECT machine_id, machine_model, machine_type, machine_description, "
                "production_year "
                "FROM v_department_machines WHERE department_id = 1");

    addQueryTab(tr("6. Site Machines"),
                "SELECT object_id, object_name, object_type, site_id, site_address, "
                "machine_id, machine_model, "
                "machine_type, machine_description, production_year, "
                "machine_assignment_start_date, machine_assignment_end_date "
                "FROM v_site_machines WHERE object_id = 2 "
                "AND machine_assignment_start_date <= '2025-08-19'::date "
                "AND (machine_assignment_end_date IS NULL OR machine_assignment_end_date "
                ">= '2020-01-01'::date)");

    addQueryTab(tr("7. Object Schedule"),
                "SELECT object_id, object_name, object_planned_start, "
                "object_planned_end, object_actual_start, object_actual_end, "
                "work_schedule_id, work_type, work_planned_start, work_planned_end, "
                "work_actual_start, work_actual_end, "
                "brigade_id, brigade_name FROM v_object_schedule WHERE object_id = 1 "
                "ORDER BY work_planned_start ASC");

    addQueryTab(tr("7. Object Budget"),
                "SELECT object_id, object_name, work_schedule_id, work_type, "
                "material_id, material_name, material_unit, "
                "material_price, material_planned_quantity, material_actual_quantity, "
                "planned_material_cost, actual_material_cost "
                "FROM v_object_budget WHERE object_id = 1");

    addQueryTab(tr("8. Construction Report"),
                "SELECT * FROM v_object_construction_report");

    addQueryTab(tr("9. Objects by Work Type"),
                "SELECT DISTINCT object_id, object_name, object_type, site_id, "
                "site_address, department_id, department_name "
                "FROM v_object_schedule WHERE work_type = 'Excavation' "
                "AND work_actual_start <= '2025-03-31'::date "
                "AND (work_actual_end IS NULL OR work_actual_end >= '2022-01-01'::date) "
                "AND department_id = 2");

    addQueryTab(tr("10. Overdue Work Types"),
                "SELECT DISTINCT work_type FROM v_object_schedule "
                "WHERE work_actual_end IS NOT NULL AND work_planned_end IS NOT NULL AND "
                "work_actual_end > work_planned_end");

    addQueryTab(
        tr("11. Over-Budget Materials"),
        "SELECT DISTINCT material_id, material_name, material_unit, material_price "
        "FROM v_object_budget WHERE material_actual_quantity > "
        "material_planned_quantity");

    addQueryTab(
        tr("12. Brigade Work History"),
        "SELECT DISTINCT work_type, object_id, object_name FROM v_object_schedule "
        "WHERE brigade_id = 5 AND work_actual_start <= '2024-12-31'::date "
        "AND (work_actual_end IS NULL OR work_actual_end >= '2024-01-01'::date)");

    addQueryTab(
        tr("13. Brigades by Work Type"),
        "SELECT DISTINCT brigade_id, brigade_name, object_id, object_name FROM "
        "v_object_schedule "
        "WHERE work_type = 'Excavation' AND work_actual_start <= '2024-12-31'::date "
        "AND (work_actual_end IS NULL OR work_actual_end >= '2024-01-01'::date)");
}

void QueryView::addQueryTab(const QString &title, const QString &query)
{
    FilterableQueryView *queryView = new FilterableQueryView(query, title, this);
    queryViews.append(queryView);
    tabWidget->addTab(queryView, title);
}

void QueryView::setDatabase(const QSqlDatabase &database)
{
    for (FilterableQueryView *view : queryViews) {
        view->setDatabase();
    }
}

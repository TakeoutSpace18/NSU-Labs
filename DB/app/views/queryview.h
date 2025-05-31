#ifndef QUERYVIEW_H
#define QUERYVIEW_H

#include <QWidget>
#include <QTabWidget>
#include <QTableView>
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDateEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QSqlQuery>
#include <QMessageBox>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QCheckBox>
#include <QScrollArea>

struct QueryParameter {
    QString name;
    QString type;
    QVariant defaultValue;
    QVariant minValue;
    QVariant maxValue;
    QStringList options; // For combo boxes
};

class FilterableQueryView : public QWidget
{
    Q_OBJECT
    
public:
    explicit FilterableQueryView(const QString &queryText, const QString &title, QWidget *parent = nullptr);
    void setDatabase();
    void executeQuery();
    
private slots:
    void onParameterChanged();
    void resetParameters();
    
private:
    void setupUI();
    void parseQueryParameters();
    void createParameterControls();
    QWidget* createParameterWidget(const QueryParameter &param);
    QString buildParameterizedQuery();
    QVariant getParameterValue(const QString &paramName);
    void setDefaultParameterValues();
    
    QString baseQuery;
    QString queryTitle;
    
    QTableView *tableView;
    QSqlQueryModel *model;
    QSortFilterProxyModel *proxyModel;
    
    QPushButton *executeButton;
    QPushButton *resetButton;
    QScrollArea *parameterScrollArea;
    QWidget *parameterWidget;
    QFormLayout *parameterLayout;
    
    QMap<QString, QueryParameter> parameters;
    QMap<QString, QWidget*> parameterControls;
    
    bool hasParameters;
};

class QueryView : public QWidget
{
    Q_OBJECT
    
public:
    explicit QueryView(QWidget *parent = nullptr);
    void setDatabase(const QSqlDatabase &database);
    
private:
    void setupQueries();
    void addQueryTab(const QString &title, const QString &query);
    
    QTabWidget *tabWidget;
    QList<FilterableQueryView*> queryViews;
};

#endif // QUERYVIEW_H

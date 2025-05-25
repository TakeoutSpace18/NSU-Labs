#ifndef MATERIALSVIEW_H
#define MATERIALSVIEW_H

#include "baseview.h"
#include <QTabWidget>

class MaterialsView : public QWidget
{
    Q_OBJECT
    
public:
    explicit MaterialsView(QWidget *parent = nullptr);
    void setDatabase(const QSqlDatabase &database);
    
private:
    QTabWidget *tabWidget;
    BaseView *materialView;
    BaseView *materialSupplyView;
};

#endif // MATERIALSVIEW_H
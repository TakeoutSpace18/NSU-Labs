#ifndef OBJECTVIEW_H
#define OBJECTVIEW_H

#include "baseview.h"
#include <QTabWidget>

class ObjectView : public QWidget
{
    Q_OBJECT
    
public:
    explicit ObjectView(QWidget *parent = nullptr);
    void setDatabase(const QSqlDatabase &database);
    
private:
    QTabWidget *tabWidget;
    BaseView *generalObjectView;
    BaseView *bridgeObjectView;
    BaseView *residentialObjectView;
    BaseView *roadObjectView;
    BaseView *hospitalObjectView;
    BaseView *schoolObjectView;
};

#endif // OBJECTVIEW_H
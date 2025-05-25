#ifndef BRIGADESVIEW_H
#define BRIGADESVIEW_H

#include "baseview.h"
#include <QTabWidget>

class BrigadesView : public QWidget
{
    Q_OBJECT
    
public:
    explicit BrigadesView(QWidget *parent = nullptr);
    void setDatabase(const QSqlDatabase &database);
    
private:
    QTabWidget *tabWidget;
    BaseView *brigadeView;
    BaseView *brigadeWorkerView;
    BaseView *brigadierView;
};

#endif // BRIGADESVIEW_H
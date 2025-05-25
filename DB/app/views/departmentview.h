#ifndef DEPARTMENTVIEW_H
#define DEPARTMENTVIEW_H

#include <qdialog.h>
#include <qsqlquery.h>
#include "baseview.h"

class DepartmentView : public BaseView
{
    Q_OBJECT
    
public:
    explicit DepartmentView(QWidget *parent = nullptr);
    
protected:
    virtual QDialog* createRecordDialog(const QSqlRecord &record, bool isNew) override;

    virtual QSqlQuery insertQuery(const QDialog *dialog) const override;
    virtual QSqlQuery updateQuery(const QDialog *dialog) const override;
};

#endif // DEPARTMENTVIEW_H

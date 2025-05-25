#ifndef CUSTOMERVIEW_H
#define CUSTOMERVIEW_H

#include "baseview.h"

class CustomerView : public BaseView
{
    Q_OBJECT
    
public:
    explicit CustomerView(QWidget *parent = nullptr);
    
protected:
    virtual QDialog* createRecordDialog(const QSqlRecord &record, bool isNew) override;
    
    virtual QSqlQuery insertQuery(const QDialog *dialog) const override;
    virtual QSqlQuery updateQuery(const QDialog *dialog) const override;

protected slots:
    virtual void _addRecord() override;
    virtual void _editRecord() override;
};

#endif // CUSTOMERVIEW_H

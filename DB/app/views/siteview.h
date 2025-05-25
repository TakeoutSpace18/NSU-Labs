#ifndef SITEVIEW_H
#define SITEVIEW_H

#include "baseview.h"

class SiteView : public BaseView
{
    Q_OBJECT
    
public:
    explicit SiteView(QWidget *parent = nullptr);
    
protected:
    virtual QDialog* createRecordDialog(const QSqlRecord &record, bool isNew) override;
    
    virtual QSqlQuery insertQuery(const QDialog *dialog) const override;
    virtual QSqlQuery updateQuery(const QDialog *dialog) const override;

protected slots:
    virtual void _addRecord() override;
    virtual void _editRecord() override;
};

#endif // SITEVIEW_H

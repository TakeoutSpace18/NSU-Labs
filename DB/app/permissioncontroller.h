#ifndef PERMISSIONCONTROLLER_H
#define PERMISSIONCONTROLLER_H

#include <QObject>
#include <QMap>
#include <QString>

class PermissionController : public QObject
{
    Q_OBJECT
    
public:
    static PermissionController& instance();
    
    enum Permission {
        NoPermission = 0,
        ReadPermission = 1,
        WritePermission = 2,
        DeletePermission = 4,
        FullPermission = 7  // Read + Write + Delete
    };
    
    void setUserRole(const QString &role);
    bool canRead(const QString &entity) const;
    bool canWrite(const QString &entity) const;
    bool canDelete(const QString &entity) const;
    
private:
    explicit PermissionController(QObject *parent = nullptr);
    void setupPermissions();
    
    QString currentRole;
    QMap<QString, QMap<QString, int>> rolePermissions;
};

#endif // PERMISSIONCONTROLLER_H
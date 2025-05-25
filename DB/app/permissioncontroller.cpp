#include "permissioncontroller.h"

PermissionController &PermissionController::instance()
{
    static PermissionController instance;
    return instance;
}

PermissionController::PermissionController(QObject *parent) : QObject(parent)
{
    setupPermissions();
}

void PermissionController::setupPermissions()
{
    // Director: Full access to everything
    QMap<QString, int> directorPermissions;
    const QStringList allEntities = {"department",
                                     "site",
                                     "customer",
                                     "object",
                                     "bridge_object",
                                     "residential_object",
                                     "road_object",
                                     "hospital_object",
                                     "school_object",
                                     "employee",
                                     "grade",
                                     "worker",
                                     "worker_type",
                                     "specialist",
                                     "specialist_type",
                                     "brigade",
                                     "work_type",
                                     "work_schedule",
                                     "material",
                                     "material_supply",
                                     "department_supervisor",
                                     "site_supervisor",
                                     "site_specialist",
                                     "brigade_worker",
                                     "brigadier",
                                     "machine_type",
                                     "machine_model",
                                     "machine",
                                     "site_machine"};

    for (const QString &entity : allEntities) {
        directorPermissions[entity] = FullPermission;
    }
    rolePermissions["director"] = directorPermissions;

    // HR Manager: Read all + Write/Delete employee, worker, specialist stuff
    QMap<QString, int> hrManagerPermissions;
    const QStringList hrWriteEntities = {"employee", "worker", "worker_type",
                                         "specialist", "specialist_type"};
    for (const QString &entity : allEntities) {
        if (hrWriteEntities.contains(entity)) {
            hrManagerPermissions[entity] =
                ReadPermission | WritePermission | DeletePermission;
        }
        else {
            hrManagerPermissions[entity] = ReadPermission;
        }
    }
    rolePermissions["hr_manager"] = hrManagerPermissions;

    // Project Manager: Read all + Write/Delete on specific site-related tables
    QMap<QString, int> projectManagerPermissions;
    const QStringList pmWriteEntities = {"site",
                                         "brigade",
                                         "work_schedule",
                                         "work_type",
                                         "material_supply",
                                         "site_supervisor",
                                         "site_specialist",
                                         "brigade_worker",
                                         "brigadier",
                                         "site_machine"};
    for (const QString &entity : allEntities) {
        if (pmWriteEntities.contains(entity)) {
            projectManagerPermissions[entity] =
                ReadPermission | WritePermission | DeletePermission;
        }
        else {
            projectManagerPermissions[entity] = ReadPermission;
        }
    }
    rolePermissions["project_manager"] = projectManagerPermissions;
}

void PermissionController::setUserRole(const QString &role)
{
    currentRole = role;
}

bool PermissionController::canRead(const QString &entity) const
{
    if (!rolePermissions.contains(currentRole) ||
        !rolePermissions[currentRole].contains(entity)) {
        return false;
    }
    return (rolePermissions[currentRole][entity] & ReadPermission) != 0;
}

bool PermissionController::canWrite(const QString &entity) const
{
    if (!rolePermissions.contains(currentRole) ||
        !rolePermissions[currentRole].contains(entity)) {
        return false;
    }
    return (rolePermissions[currentRole][entity] & WritePermission) != 0;
}

bool PermissionController::canDelete(const QString &entity) const
{
    if (!rolePermissions.contains(currentRole) ||
        !rolePermissions[currentRole].contains(entity)) {
        return false;
    }
    return (rolePermissions[currentRole][entity] & DeletePermission) != 0;
}

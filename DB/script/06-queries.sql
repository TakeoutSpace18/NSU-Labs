-- 1. Получить перечень строительных управлений и их текущих руководителей.
SELECT
    department_id,
    department_name,
    employee_id,
    name,
    surname,
    middlename,
    start_date
FROM
    v_departments_supervisors
WHERE
    end_date is NULL;

-- 1. Получить перечень строительных участков и их текущих руководителей.
SELECT
    site_id,
    address AS site_address,
    department_name,
    employee_id,
    name,
    surname,
    middlename,
    start_date
FROM v_sites_supervisors
WHERE end_date IS NULL;

-- 2. Получить список специалистов инженерно-технического состава обозначенного
-- участка или строительного управления с указанием их должностей.

SELECT
    employee_id,
    name,
    surname,
    middlename,
    position,
    specialist_type,
    specialist_description,
    site_assignment_start_date
FROM v_site_department_specialists
WHERE site_id = 9
    AND site_assignment_end_date is NULL;

-- 3. Получить перечень объектов, возводимых указанным строительным управлением
-- и/или участком, и графики их возведения.

SELECT 
    object_id,
    object_name,
    object_type,
    site_address,
    department_name,
    customer_name,
    planned_start_date,
    planned_end_date,
    actual_start_date,
    actual_end_date
FROM v_department_site_objects
WHERE site_id = 20;

-- 4. Получить состав бригад, работавших (работающих) на строительстве
-- указанного объекта.

SELECT 
    brigade_id,
    brigade_name,
    employee_id,
    employee_name,
    employee_surname,
    employee_middlename,
    worker_position,
    worker_type,
    brigade_assignment_start_date,
    brigade_assignment_end_date
FROM v_object_brigades
WHERE object_id = 1
ORDER BY brigade_name;

-- 5. Получить перечень строительной техники, приданной указанному строительному
-- управлению.

SELECT 
    machine_id,
    machine_model,
    machine_type,
    machine_description,
    production_year
FROM v_department_machines
WHERE department_id = 1;

-- 6. Получить перечень строительной техники, выделенной на указанный объект
-- либо работавшей там в течение указанного периода времени.

SELECT 
    object_id,
    object_name,
    object_type,
    site_id,
    site_address,
    machine_id,
    machine_model,
    machine_type,
    machine_description,
    production_year,
    machine_assignment_start_date,
    machine_assignment_end_date
FROM v_site_machines
WHERE object_id = 2
    AND machine_assignment_start_date <= '2025-08-19'
    AND (machine_assignment_end_date IS NULL OR machine_assignment_end_date >= '2020-01-01');

-- 7. Получить график на строительство указанного объекта.
SELECT 
    object_id,
    object_name,
    object_planned_start,
    object_planned_end,
    object_actual_start,
    object_actual_end,
    work_schedule_id,
    work_type,
    work_planned_start,
    work_planned_end,
    work_actual_start,
    work_actual_end,
    brigade_id,
    brigade_name
FROM v_object_schedule
WHERE object_id = 1
ORDER BY work_planned_start ASC;

-- 7. Получить смету на строительство указанного объекта.

SELECT 
    object_id,
    object_name,
    work_schedule_id,
    work_type,
    material_id,
    material_name,
    material_unit,
    material_price,
    material_planned_quantity,
    material_actual_quantity,
    planned_material_cost,
    actual_material_cost
FROM v_object_budget
WHERE object_id = 1;

-- 8. Получить отчет о сооружении указанного объекта.

SELECT * FROM v_object_construction_report;

-- 9. Получить перечень объектов, возводимых в некотором строительном управлении
-- или в целом по организации, на которых в обозначенный период времени
-- выполнялся указанный вид строительных работ.

SELECT DISTINCT
    object_id,
    object_name,
    object_type,
    site_id,
    site_address,
    department_id,
    department_name
FROM v_object_schedule
WHERE work_type = 'Excavation'
    AND work_actual_start <= '2025-03-31'
    AND (work_actual_end IS NULL OR work_actual_end >= '2022-01-01')
    AND department_id = 2;

-- 10. Получить перечень видов строительных работ, по которым имело место
-- превышение сроков выполнения на указанном участке, строительном управлении
-- или в целом по организации.

SELECT DISTINCT
    work_type
FROM v_object_schedule
WHERE work_actual_end IS NOT NULL
    AND work_planned_end IS NOT NULL
    AND work_actual_end > work_planned_end;

-- 11. Получить перечень строительных материалов, по которым имело место
-- превышение по смете на указанном участке, строительном управлении или в целом
-- по организации.

SELECT DISTINCT
    material_id,
    material_name,
    material_unit,
    material_price
FROM v_object_budget
WHERE material_actual_quantity > material_planned_quantity;

-- 12. Получить перечень видов строительных работ, выполненных указанной
-- бригадой в течение обозначенного периода времени с указанием объектов, где
-- эти работы выполнялись.

SELECT DISTINCT
    work_type,
    object_id,
    object_name
FROM v_object_schedule
WHERE brigade_id = 5
    AND work_actual_start <= '2024-12-31'
    AND (work_actual_end IS NULL OR work_actual_end >= '2024-01-01');

-- 13. Получить перечень бригад, выполненных указанный вид строительных работ в
-- течение обозначенного периода времени с указанием объектов, где эти работы
-- выполнялись.

SELECT DISTINCT
    brigade_id,
    brigade_name,
    object_id,
    object_name
FROM v_object_schedule os
WHERE os.work_type = 'Excavation'
    AND os.work_actual_start <= '2024-12-31'
    AND (os.work_actual_end IS NULL OR os.work_actual_end >= '2024-01-01');


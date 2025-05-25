# Отчет по запросам к базе данных

## Представления

[Скрипт с представлениями](../script/05-views.sql)

### 1. `v_departments_supervisors`
Представление возвращает перечень строительных управлений и их руководителей.

```sql
CREATE OR REPLACE VIEW v_departments_supervisors AS
SELECT
    d.department_id,
    d.name AS department_name,
    e.employee_id,
    e.name,
    e.surname,
    e.middlename,
    ds.start_date,
    ds.end_date
FROM department d
LEFT JOIN department_supervisor ds ON d.department_id = ds.department_id 
JOIN employee e ON ds.specialist_id = e.employee_id;
```

### 2. `v_sites_supervisors`
Представление возвращает перечень строительных участков и их руководителей.

```sql
CREATE OR REPLACE VIEW v_sites_supervisors AS
SELECT
    s.site_id,
    s.address,
    d.department_id,
    d.name AS department_name,
    e.employee_id,
    e.name,
    e.surname,
    e.middlename,
    ss.start_date,
    ss.end_date
FROM site s
LEFT JOIN department d ON s.department_id = d.department_id
LEFT JOIN site_supervisor ss ON s.site_id = ss.site_id 
JOIN employee e ON ss.specialist_id = e.employee_id;
```

### 3. `v_site_department_specialists`
Представление возвращает список специалистов инженерно-технического состава с указанием их должностей.

```sql
CREATE OR REPLACE VIEW v_site_department_specialists AS
SELECT 
    e.employee_id,
    e.name,
    e.surname,
    e.middlename,
    e.phone,
    e.email,
    e.salary,
    st.type AS specialist_type,
    st.description AS specialist_description,
    s.site_id,
    s.address AS site_address,
    d.department_id,
    d.name AS department_name,
    ss.start_date AS site_assignment_start_date,
    ss.end_date AS site_assignment_end_date,
    CASE 
        WHEN ds.specialist_id IS NOT NULL THEN 'Department Supervisor'
        WHEN site_sup.specialist_id IS NOT NULL THEN 'Site Supervisor'
        ELSE 'Specialist'
    END AS position
FROM employee e
JOIN specialist sp ON e.employee_id = sp.employee_id
JOIN specialist_type st ON sp.specialist_type_id = st.specialist_type_id
LEFT JOIN site_specialist ss ON sp.employee_id = ss.specialist_id
JOIN site s ON ss.site_id = s.site_id
JOIN department d ON s.department_id = d.department_id
LEFT JOIN department_supervisor ds ON sp.employee_id = ds.specialist_id AND d.department_id = ds.department_id
LEFT JOIN site_supervisor site_sup ON sp.employee_id = site_sup.specialist_id AND s.site_id = site_sup.site_id;
```

### 4. `v_department_site_objects`
Представление возвращает список объектов с их строительным участком, управлением и заказчиком.

```sql

CREATE OR REPLACE VIEW v_department_site_objects AS
SELECT 
    o.object_id,
    o.name AS object_name,
    o.type AS object_type,
    o.planned_start_date,
    o.planned_end_date,
    o.actual_start_date,
    o.actual_end_date,
    s.site_id,
    s.address AS site_address,
    d.department_id,
    d.name AS department_name,
    c.customer_id,
    c.name AS customer_name
FROM object o
JOIN site s ON o.site_id = s.site_id
JOIN department d ON s.department_id = d.department_id
JOIN customer c ON o.customer_id = c.customer_id;
```

### 5. `v_object_brigades`
Представление возвращает состав бригад, работавших (работающих) на строительстве указанного объекта.

```sql
CREATE OR REPLACE VIEW v_object_brigades AS
SELECT 
    o.object_id,
    o.name AS object_name,
    o.type AS object_type,
    b.brigade_id,
    b.name AS brigade_name,
    ws.work_schedule_id,
    ws.planned_start_date,
    ws.planned_end_date,
    ws.actual_start_date,
    ws.actual_end_date,
    wt.work_type_id,
    wt.name AS work_type_name,
    wt.description AS work_type_description,
    e.employee_id,
    e.name AS employee_name,
    e.surname AS employee_surname,
    e.middlename AS employee_middlename,
    e.phone AS employee_phone,
    e.email AS employee_email,
    wkt.type AS worker_type,
    bw.start_date AS brigade_assignment_start_date,
    bw.end_date AS brigade_assignment_end_date,
    CASE 
        WHEN br.worker_id IS NOT NULL THEN 'Brigadier'
        ELSE 'Worker'
    END AS worker_position
FROM object o
JOIN work_schedule ws ON o.object_id = ws.object_id
JOIN work_type wt ON ws.work_type_id = wt.work_type_id
JOIN brigade b ON ws.brigade_id = b.brigade_id
JOIN brigade_worker bw ON b.brigade_id = bw.brigade_id
JOIN worker w ON bw.worker_id = w.employee_id
JOIN employee e ON w.employee_id = e.employee_id
JOIN worker_type wkt ON w.worker_type_id = wkt.worker_type_id
LEFT JOIN brigadier br ON b.brigade_id = br.brigade_id;
```

### 6. `v_department_machines`
Представление возвращает перечень строительной техники, приданной указанному строительному управлению.

```sql
CREATE OR REPLACE VIEW v_department_machines AS
SELECT 
    d.department_id,
    d.name AS department_name,
    m.machine_id,
    mm.model AS machine_model,
    mt.type AS machine_type,
    mt.description AS machine_description,
    m.production_year
FROM department d
JOIN machine m ON d.department_id = m.department_id
JOIN machine_model mm ON m.machine_model_id = mm.machine_model_id
JOIN machine_type mt ON mm.machine_type_id = mt.machine_type_id;
```

### 7. `v_site_machines`
Представление возвращает перечень строительной техники, выделенной на указанный объект.

```sql
CREATE OR REPLACE VIEW v_site_machines AS
SELECT 
    o.object_id,
    o.name AS object_name,
    o.type AS object_type,
    s.site_id,
    s.address AS site_address,
    d.department_id,
    d.name AS department_name,
    m.machine_id,
    mm.model AS machine_model,
    mt.type AS machine_type,
    mt.description AS machine_description,
    m.production_year,
    sm.start_date AS machine_assignment_start_date,
    sm.end_date AS machine_assignment_end_date
FROM object o
JOIN site s ON o.site_id = s.site_id
JOIN department d ON s.department_id = d.department_id
JOIN site_machine sm ON s.site_id = sm.site_id
JOIN machine m ON sm.machine_id = m.machine_id
JOIN machine_model mm ON m.machine_model_id = mm.machine_model_id
JOIN machine_type mt ON mm.machine_type_id = mt.machine_type_id;
```

### 8. `v_object_schedule`
Представление возвращает график на строительство указанного объекта.

```sql
CREATE OR REPLACE VIEW v_object_schedule AS
SELECT 
    o.object_id,
    o.name AS object_name,
    o.type AS object_type,
    o.planned_start_date AS object_planned_start,
    o.planned_end_date AS object_planned_end,
    o.actual_start_date AS object_actual_start,
    o.actual_end_date AS object_actual_end,
    s.site_id,
    s.address AS site_address,
    d.department_id,
    d.name AS department_name,
    ws.work_schedule_id,
    wt.name AS work_type,
    ws.planned_start_date AS work_planned_start,
    ws.planned_end_date AS work_planned_end,
    ws.actual_start_date AS work_actual_start,
    ws.actual_end_date AS work_actual_end,
    b.brigade_id,
    b.name AS brigade_name
FROM object o
JOIN site s ON o.site_id = s.site_id
JOIN department d ON s.department_id = d.department_id
JOIN work_schedule ws ON o.object_id = ws.object_id
JOIN work_type wt ON ws.work_type_id = wt.work_type_id
LEFT JOIN brigade b ON ws.brigade_id = b.brigade_id;
```

### 9. `v_object_budget`
Представление возвращает смету на строительство указанного объекта.

```sql
CREATE OR REPLACE VIEW v_object_budget AS
SELECT 
o.object_id,
    o.name AS object_name,
    o.type AS object_type,
    s.site_id,
    s.address AS site_address,
    d.department_id,
    d.name AS department_name,
    ws.work_schedule_id,
    wt.name AS work_type,
    m.material_id,
    m.name AS material_name,
    m.unit AS material_unit,
    m.price AS material_price,
    ms.planned_quantity AS material_planned_quantity,
    ms.actual_quantity AS material_actual_quantity,
    (m.price * ms.planned_quantity) AS planned_material_cost,
    (m.price * ms.actual_quantity) AS actual_material_cost
FROM object o
JOIN site s ON o.site_id = s.site_id
JOIN department d ON s.department_id = d.department_id
JOIN work_schedule ws ON o.object_id = ws.object_id
JOIN work_type wt ON ws.work_type_id = wt.work_type_id
JOIN material_supply ms ON ws.work_schedule_id = ms.work_schedule_id
JOIN material m ON ms.material_id = m.material_id;
```

### 10. `v_object_construction_report`
Представление возвращает отчет о сооружении указанного объекта.

```sql
CREATE OR REPLACE VIEW v_object_construction_report AS
SELECT 
    o.object_id,
    o.name AS object_name,
    o.type AS object_type,
    o.planned_start_date,
    o.planned_end_date,
    o.actual_start_date,
    o.actual_end_date,
    CASE 
        WHEN o.actual_end_date IS NULL THEN 'In Progress'
        WHEN o.actual_end_date <= o.planned_end_date THEN 'Completed On Time'
        ELSE 'Completed With Delay'
    END AS project_status,
    CASE 
        WHEN o.actual_end_date IS NOT NULL THEN 
            o.actual_end_date - o.actual_start_date
        ELSE 
            CURRENT_DATE - o.actual_start_date
    END AS actual_duration_days,
    CASE 
        WHEN o.planned_end_date IS NOT NULL AND o.planned_start_date IS NOT NULL THEN 
            o.planned_end_date - o.planned_start_date
    END AS planned_duration_days,
    s.site_id,
    s.address AS site_address,
    d.department_id,
    d.name AS department_name,
    c.customer_id,
    c.name AS customer_name,
    (SELECT COUNT(ws.work_schedule_id) 
     FROM work_schedule ws 
     WHERE ws.object_id = o.object_id) AS total_works_count,
    (SELECT COUNT(ws.work_schedule_id) 
     FROM work_schedule ws 
     WHERE ws.object_id = o.object_id 
     AND ws.actual_end_date IS NOT NULL) AS completed_works_count,
    (SELECT SUM(m.price * ms.planned_quantity)
     FROM work_schedule ws
     JOIN material_supply ms ON ws.work_schedule_id = ms.work_schedule_id
     JOIN material m ON ms.material_id = m.material_id
     WHERE ws.object_id = o.object_id) AS total_planned_material_cost,
    (SELECT SUM(m.price * ms.actual_quantity)
     FROM work_schedule ws
     JOIN material_supply ms ON ws.work_schedule_id = ms.work_schedule_id
     JOIN material m ON ms.material_id = m.material_id
     WHERE ws.object_id = o.object_id) AS total_actual_material_cost
FROM object o
JOIN site s ON o.site_id = s.site_id
JOIN department d ON s.department_id = d.department_id
JOIN customer c ON o.customer_id = c.customer_id;
```

## Запросы

[Скрипт с запросами](../script/06-queries.sql)

### Запрос 1
Получить перечень строительных управлений и их текущих руководителей.\
Использует представление [v_departments_supervisors](#1.-v_departments_supervisors).

```sql
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
```

### Запрос 2
Получить перечень строительных участков и их текущих руководителей.\
Использует представление [v_sites_supervisors](#2.-v_sites_supervisors).

```sql
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
```

### Запрос 3
Получить список специалистов инженерно-технического состава обозначенного участка или строительного управления с указанием их должностей.\
Использует представление [v_site_department_specialists](#3.-v_site_department_specialists).

```sql
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
```

### Запрос 4
Получить перечень объектов, возводимых указанным строительным управлением и/или участком, и графики их возведения.\
Использует представление [v_department_site_objects](#4-v_department_site_objects).

```sql
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
```

### Запрос 5
Получить состав бригад, работавших (работающих) на строительстве указанного объекта.\
Использует представление [v_object_brigades](#5.-v_object_brigades).

```sql
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
```

### Запрос 6
Получить перечень строительной техники, приданной указанному строительному управлению.\
Использует представление [v_department_machines](#6.-v_department_machines).

```sql
SELECT 
    machine_id,
    machine_model,
    machine_type,
    machine_description,
    production_year
FROM v_department_machines
WHERE department_id = 1;
```

### Запрос 7
Получить перечень строительной техники, выделенной на указанный объект либо работавшей там в течение указанного периода времени.\
Использует представление [v_site_machines](#7.-v_site_machines).

```sql
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
    AND machine_assignment_start_date <= '2025-08-19'::date
    AND (machine_assignment_end_date IS NULL OR machine_assignment_end_date >= '2020-01-01'::date);
```

### Запрос 8
Получить график на строительство указанного объекта.\
Использует представление [v_object_schedule](#8.-v_object_schedule).

```sql
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
```

### Запрос 9
Получить смету на строительство указанного объекта.\
Использует представление [v_object_budget](#9.-v_object_budget).

```sql
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
```

### Запрос 10
Получить отчет о сооружении указанного объекта.\
Использует представление [v_object_construction_report](#10.-v_object_construction_report).

```sql
SELECT * FROM v_object_construction_report;
```

### Запрос 11
Получить перечень объектов, возводимых в некотором строительном управлении или в целом по организации, на которых в обозначенный период времени выполнялся указанный вид строительных работ.\
Использует представление [v_object_schedule](#8.-v_object_schedule).

```sql
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
    AND work_actual_start <= '2025-03-31'::date
    AND (work_actual_end IS NULL OR work_actual_end >= '2022-01-01'::date)
    AND department_id = 2;
```

### Запрос 12
Получить перечень видов строительных работ, по которым имело место превышение сроков выполнения на указанном участке, строительном управлении или в целом по организации.\
Использует представление [v_object_schedule](#8.-v_object_schedule).

```sql
SELECT DISTINCT
    work_type
FROM v_object_schedule
WHERE work_actual_end IS NOT NULL
    AND work_planned_end IS NOT NULL
    AND work_actual_end > work_planned_end;
```

### Запрос 13
Получить перечень строительных материалов, по которым имело место превышение по смете на указанном участке, строительном управлении или в целом по организации.\
Использует представление [v_object_budget](#9.-v_object_budget).

```sql
SELECT DISTINCT
    material_id,
    material_name,
    material_unit,
    material_price
FROM v_object_budget
WHERE material_actual_quantity > material_planned_quantity;
```

### Запрос 14
Получить перечень видов строительных работ, выполненных указанной бригадой в течение обозначенного периода времени с указанием объектов, где эти работы выполнялись.\
Использует представление [v_object_schedule](#8.-v_object_schedule).

```sql
SELECT DISTINCT
    work_type,
    object_id,
    object_name
FROM v_object_schedule
WHERE brigade_id = 5
    AND work_actual_start <= '2024-12-31'::date
    AND (work_actual_end IS NULL OR work_actual_end >= '2024-01-01'::date);
```

### Запрос 15
Получить перечень бригад, выполненных указанный вид строительных работ в течение обозначенного периода времени с указанием объектов, где эти работы выполнялись.\
Использует представление [v_object_schedule](#8.-v_object_schedule).

```sql
SELECT DISTINCT
    brigade_id,
    brigade_name,
    object_id,
    object_name
FROM v_object_schedule os
WHERE os.work_type = 'Excavation'
    AND os.work_actual_start <= '2024-12-31'::date
    AND (os.work_actual_end IS NULL OR os.work_actual_end >= '2024-01-01'::date);
```

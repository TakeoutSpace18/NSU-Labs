-- recreate roles if they are already created
DROP ROLE IF EXISTS director;
DROP ROLE IF EXISTS hr_manager;
DROP ROLE IF EXISTS project_manager;

CREATE ROLE director WITH LOGIN PASSWORD 'director';
CREATE ROLE hr_manager WITH LOGIN PASSWORD 'hr_manager';
CREATE ROLE project_manager WITH LOGIN PASSWORD 'project_manager';

GRANT CONNECT ON DATABASE cfdb TO director, hr_manager, project_manager;


-- Role: Director (has full access to all entities)
GRANT USAGE ON SCHEMA public TO director;
GRANT SELECT, INSERT, UPDATE, DELETE ON ALL TABLES IN SCHEMA public TO director;
GRANT USAGE, SELECT ON ALL SEQUENCES IN SCHEMA public TO director;

-- Role: HR Manager
GRANT USAGE ON SCHEMA public TO hr_manager;
GRANT SELECT ON ALL TABLES IN SCHEMA public TO hr_manager;
GRANT INSERT, UPDATE, DELETE ON 
    employee, 
    worker, 
    worker_type, 
    specialist, 
    specialist_type 
TO hr_manager;

-- Grant sequence usage for tables with CRUD access
GRANT USAGE, SELECT ON SEQUENCE 
    employee_employee_id_seq, 
    worker_type_worker_type_id_seq,
    specialist_type_specialist_type_id_seq 
TO hr_manager;


-- Role: Project Manager
GRANT USAGE ON SCHEMA public TO project_manager;
GRANT SELECT ON ALL TABLES IN SCHEMA public TO project_manager;
GRANT INSERT, UPDATE, DELETE ON
    site,
    brigade,
    work_schedule,
    work_type,
    material_supply,
    site_supervisor,
    site_specialist,
    brigade_worker,
    brigadier,
    site_machine
TO project_manager;

-- Grant sequence usage for tables with CRUD access
GRANT USAGE, SELECT ON SEQUENCE
    site_site_id_seq,
    brigade_brigade_id_seq,
    work_schedule_work_schedule_id_seq,
    work_type_work_type_id_seq,
    material_supply_material_supply_id_seq,
    site_supervisor_site_supervisor_id_seq,
    site_specialist_site_specialist_id_seq,
    brigade_worker_brigade_worker_id_seq,
    brigadier_brigadier_id_seq,
    site_machine_site_machine_id_seq
TO project_manager;


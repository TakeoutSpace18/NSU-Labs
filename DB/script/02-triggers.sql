-- triggers and stored procedures

CREATE OR REPLACE FUNCTION check_work_schedule_dates()
RETURNS trigger
AS $$
DECLARE
    obj_planned_start DATE;
    obj_planned_end DATE;
    obj_actual_start DATE;
    obj_actual_end DATE;
    obj_name VARCHAR(100);
BEGIN
    -- Get object details including name for better error reporting
    SELECT 
        planned_start_date, 
        planned_end_date, 
        actual_start_date, 
        actual_end_date,
        name
    INTO 
        obj_planned_start, 
        obj_planned_end, 
        obj_actual_start, 
        obj_actual_end,
        obj_name
    FROM object
    WHERE object_id = NEW.object_id;
    
    -- Check planned dates
    IF NEW.planned_start_date IS NOT NULL AND obj_planned_start IS NOT NULL THEN
        IF NEW.planned_start_date < obj_planned_start THEN
            RAISE EXCEPTION 'Work schedule ID % for object "%" (ID: %) has planned start date (%) earlier than object planned start date (%)',
                NEW.work_schedule_id, obj_name, NEW.object_id, NEW.planned_start_date, obj_planned_start;
        END IF;
    END IF;
    
    IF NEW.planned_end_date IS NOT NULL AND obj_planned_end IS NOT NULL THEN
        IF NEW.planned_end_date > obj_planned_end THEN
            RAISE EXCEPTION 'Work schedule ID % for object "%" (ID: %) has planned end date (%) later than object planned end date (%)',
                NEW.work_schedule_id, obj_name, NEW.object_id, NEW.planned_end_date, obj_planned_end;
        END IF;
    END IF;
    
    -- Check actual dates
    IF NEW.actual_start_date IS NOT NULL AND obj_actual_start IS NOT NULL THEN
        IF NEW.actual_start_date < obj_actual_start THEN
            RAISE EXCEPTION 'Work schedule ID % for object "%" (ID: %) has actual start date (%) earlier than object actual start date (%)',
                NEW.work_schedule_id, obj_name, NEW.object_id, NEW.actual_start_date, obj_actual_start;
        END IF;
    END IF;
    
    IF NEW.actual_end_date IS NOT NULL AND obj_actual_end IS NOT NULL THEN
        IF NEW.actual_end_date > obj_actual_end THEN
            RAISE EXCEPTION 'Work schedule ID % for object "%" (ID: %) has actual end date (%) later than object actual end date (%)',
                NEW.work_schedule_id, obj_name, NEW.object_id, NEW.actual_end_date, obj_actual_end;
        END IF;
    END IF;
    
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION check_object_date_constraints()
RETURNS trigger
AS $$
DECLARE
    ws_record RECORD;
BEGIN
    -- only check if dates are being updated
    IF (
        NEW.planned_start_date IS DISTINCT FROM OLD.planned_start_date OR
        NEW.planned_end_date IS DISTINCT FROM OLD.planned_end_date OR
        NEW.actual_start_date IS DISTINCT FROM OLD.actual_start_date OR
        NEW.actual_end_date IS DISTINCT FROM OLD.actual_end_date
    ) THEN
        -- check all work_schedules for this object
        FOR ws_record IN 
            SELECT * FROM work_schedule WHERE object_id = NEW.object_id
        LOOP
            -- check planned dates
            IF ws_record.planned_start_date IS NOT NULL AND NEW.planned_start_date IS NOT NULL THEN
                IF ws_record.planned_start_date < NEW.planned_start_date THEN
                    RAISE EXCEPTION 'Cannot update object: work schedule ID % has planned start date earlier than new object planned start date', ws_record.work_schedule_id;
                END IF;
            END IF;

            IF ws_record.planned_end_date IS NOT NULL AND NEW.planned_end_date IS NOT NULL THEN
                IF ws_record.planned_end_date > NEW.planned_end_date THEN
                    RAISE EXCEPTION 'Cannot update object: work schedule ID % has planned end date later than new object planned end date', ws_record.work_schedule_id;
                END IF;
            END IF;

            -- check actual dates
            IF ws_record.actual_start_date IS NOT NULL AND NEW.actual_start_date IS NOT NULL THEN
                IF ws_record.actual_start_date < NEW.actual_start_date THEN
                    RAISE EXCEPTION 'Cannot update object: work schedule ID % has actual start date earlier than new object actual start date', ws_record.work_schedule_id;
                END IF;
            END IF;

            IF ws_record.actual_end_date IS NOT NULL AND NEW.actual_end_date IS NOT NULL THEN
                IF ws_record.actual_end_date > NEW.actual_end_date THEN
                    RAISE EXCEPTION 'Cannot update object: work schedule ID % has actual end date later than new object actual end date', ws_record.work_schedule_id;
                END IF;
            END IF;
        END LOOP;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION check_single_brigadier()
RETURNS trigger
AS $$
DECLARE
    existing_brigadier RECORD;
BEGIN
    SELECT * INTO existing_brigadier
    FROM brigadier
    WHERE brigadier_id != COALESCE(NEW.brigadier_id, -1)
      AND brigade_id = NEW.brigade_id
      AND (end_date IS NULL OR end_date > NEW.start_date)
      AND (NEW.end_date IS NULL OR start_date < NEW.end_date);
    
    IF FOUND THEN
        RAISE EXCEPTION 'Brigade % already has an active brigadier (worker ID: %) for the period % to %', 
            NEW.brigade_id, existing_brigadier.worker_id, 
            existing_brigadier.start_date, 
            COALESCE(existing_brigadier.end_date::text, 'ongoing');
    END IF;
    
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION check_single_site_supervisor()
RETURNS trigger
AS $$
DECLARE
    existing_site_supervisor RECORD;
BEGIN
    SELECT * INTO existing_site_supervisor
    FROM site_supervisor
    WHERE specialist_id != COALESCE(NEW.specialist_id, -1)
      AND site_id = NEW.site_id
      AND (end_date IS NULL OR end_date > NEW.start_date)
      AND (NEW.end_date IS NULL OR start_date < NEW.end_date);
    
    IF FOUND THEN
        RAISE EXCEPTION 'Site % already has an active supervisor (ID: %) for the period % to %', 
            NEW.site_id, existing_site_supervisor.specialist_id, 
            existing_site_supervisor.start_date, 
            COALESCE(existing_site_supervisor.end_date::text, 'ongoing');
    END IF;
    
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION check_single_department_supervisor()
RETURNS trigger
AS $$
DECLARE
    existing_department_supervisor RECORD;
BEGIN
    SELECT * INTO existing_department_supervisor
    FROM department_supervisor
    WHERE specialist_id != COALESCE(NEW.specialist_id, -1)
      AND department_id = NEW.department_id
      AND (end_date IS NULL OR end_date > NEW.start_date)
      AND (NEW.end_date IS NULL OR start_date < NEW.end_date);
    
    IF FOUND THEN
        RAISE EXCEPTION 'department % already has an active supervisor (ID: %) for the period % to %', 
            NEW.department_id, existing_department_supervisor.specialist_id, 
            existing_department_supervisor.start_date, 
            COALESCE(existing_department_supervisor.end_date::text, 'ongoing');
    END IF;
    
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION check_worker_single_brigade()
RETURNS TRIGGER AS $$
DECLARE
    active_brigade_count INTEGER;
BEGIN
    SELECT COUNT(*) 
    INTO active_brigade_count
    FROM brigade_worker
    WHERE worker_id = NEW.worker_id
      AND (end_date IS NULL OR end_date >= NEW.start_date)
      AND (NEW.end_date IS NULL OR start_date < NEW.end_date)
      AND brigade_worker_id != COALESCE(NEW.brigade_worker_id, -1)
      AND brigade_id != NEW.brigade_id;
    
    IF active_brigade_count > 0 THEN
        RAISE EXCEPTION 'Worker ID % is already assigned to another brigade during this time period', NEW.worker_id;
    END IF;
    
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION check_specialist_single_site()
RETURNS TRIGGER AS $$
DECLARE
    active_site_count INTEGER;
BEGIN
    SELECT COUNT(*) 
    INTO active_site_count
    FROM site_specialist
    WHERE specialist_id = NEW.specialist_id
      AND (end_date IS NULL OR end_date >= NEW.start_date)
      AND (NEW.end_date IS NULL OR start_date < NEW.end_date)
      AND site_specialist_id != COALESCE(NEW.site_specialist_id, -1)
      AND site_id != NEW.site_id;
    
    IF active_site_count > 0 THEN
        RAISE EXCEPTION 'Specialist ID % is already assigned to another site during this time period', NEW.specialist_id;
    END IF;
    
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION check_brigadier_no_brigade_worker()
RETURNS TRIGGER AS $$
DECLARE
    existing_brigade_worker RECORD;
BEGIN
    SELECT * INTO existing_brigade_worker
    FROM brigade_worker bw
    WHERE bw.worker_id = NEW.worker_id
      AND (bw.end_date IS NULL OR bw.end_date > NEW.start_date)
      AND (NEW.end_date IS NULL OR bw.start_date < NEW.end_date);
    
    IF FOUND THEN
        RAISE EXCEPTION 'Worker ID % cannot be both a brigadier and a brigade worker during the same time period',
            NEW.worker_id;
    END IF;
    
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION check_brigade_worker_no_brigadier()
RETURNS TRIGGER AS $$
DECLARE
    existing_brigadier RECORD;
BEGIN
    SELECT * INTO existing_brigadier
    FROM brigadier b
    WHERE b.worker_id = NEW.worker_id
      AND (b.end_date IS NULL OR b.end_date > NEW.start_date)
      AND (NEW.end_date IS NULL OR b.start_date < NEW.end_date);
    
    IF FOUND THEN
        RAISE EXCEPTION 'Worker ID % cannot be both a brigadier and a brigade worker during the same time period',
            NEW.worker_id;
    END IF;
    
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION check_site_supervisor_no_site_specialist()
RETURNS TRIGGER AS $$
DECLARE
    existing_site_specialist RECORD;
BEGIN
    SELECT * INTO existing_site_specialist
    FROM site_specialist ss
    WHERE ss.specialist_id = NEW.specialist_id
      AND (ss.end_date IS NULL OR ss.end_date > NEW.start_date)
      AND (NEW.end_date IS NULL OR ss.start_date < NEW.end_date);
    
    IF FOUND THEN
        RAISE EXCEPTION 'Specialist ID % cannot be both a supervisor and a site specialist during the same time period',
            NEW.specialist_id;
    END IF;
    
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION check_site_specialist_no_site_supervisor()
RETURNS TRIGGER AS $$
DECLARE
    existing_site_supervisor RECORD;
BEGIN
    SELECT * INTO existing_site_supervisor
    FROM site_supervisor
    WHERE specialist_id = NEW.specialist_id
      AND (end_date IS NULL OR end_date > NEW.start_date)
      AND (NEW.end_date IS NULL OR start_date < NEW.end_date);
    
    IF FOUND THEN
        RAISE EXCEPTION 'Specialist ID % cannot be both a supervisor and a site specialist during the same time period',
            NEW.specialist_id;
    END IF;
    
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION check_worker_single_brigadier()
RETURNS TRIGGER AS $$
DECLARE
    existing_brigadier_count INTEGER;
BEGIN
    SELECT COUNT(*) INTO existing_brigadier_count
    FROM brigadier
    WHERE worker_id = NEW.worker_id
      AND (end_date IS NULL OR end_date >= NEW.start_date)
      AND (NEW.end_date IS NULL OR start_date < NEW.end_date)
      AND brigadier_id != COALESCE(NEW.brigadier_id, -1);
    
    IF existing_brigadier_count > 0 THEN
        RAISE EXCEPTION 'Worker ID % is already a brigadier for another brigade during this period', NEW.worker_id;
    END IF;
    
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION check_specialist_single_site_supervisor()
RETURNS TRIGGER AS $$
DECLARE
    existing_site_supervisor_count INTEGER;
BEGIN
    SELECT COUNT(*) INTO existing_site_supervisor_count
    FROM site_supervisor
    WHERE specialist_id = NEW.specialist_id
      AND (end_date IS NULL OR end_date >= NEW.start_date)
      AND (NEW.end_date IS NULL OR start_date < NEW.end_date)
      AND site_supervisor_id != COALESCE(NEW.site_supervisor_id, -1);
    
    IF existing_site_supervisor_count > 0 THEN
        RAISE EXCEPTION 'Specialist ID % is already a supervisor for another site during this period', NEW.worker_id;
    END IF;
    
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION check_specialist_single_department_supervisor()
RETURNS TRIGGER AS $$
DECLARE
    existing_department_supervisor_count INTEGER;
BEGIN
    SELECT COUNT(*) INTO existing_department_supervisor_count
    FROM department_supervisor
    WHERE specialist_id = NEW.specialist_id
      AND (end_date IS NULL OR end_date >= NEW.start_date)
      AND (NEW.end_date IS NULL OR start_date < NEW.end_date)
      AND department_supervisor_id != COALESCE(NEW.department_supervisor_id, -1);
    
    IF existing_department_supervisor_count > 0 THEN
        RAISE EXCEPTION 'Specialist ID % is already a supervisor for another department during this period', NEW.worker_id;
    END IF;
    
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION check_machine_single_site_assignment()
RETURNS TRIGGER AS $$
DECLARE
    existing_assignments_count INTEGER;
BEGIN
    SELECT COUNT(*) INTO existing_assignments_count
    FROM site_machine
    WHERE machine_id = NEW.machine_id
      AND (end_date IS NULL OR end_date >= NEW.start_date)
      AND (NEW.end_date IS NULL OR start_date < NEW.end_date)
      AND site_machine_id != COALESCE(NEW.site_machine_id, -1);
    
    IF existing_assignments_count > 0 THEN
        RAISE EXCEPTION 'Machine ID % is already assigned for site during this period', NEW.machine_id;
    END IF;
    
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- ensure there is no work_schedule with dates not between object construction dates
CREATE OR REPLACE TRIGGER enforce_work_schedule_date_constraints
BEFORE INSERT OR UPDATE ON work_schedule
FOR EACH ROW
    EXECUTE FUNCTION check_work_schedule_dates();


-- ensure object dates contain all work_schedule dates
CREATE OR REPLACE TRIGGER enforce_object_date_constraints
BEFORE UPDATE ON object
FOR EACH ROW
    EXECUTE FUNCTION check_object_date_constraints();


-- ensure brigade has one brigadier at a time
CREATE OR REPLACE TRIGGER enforce_single_brigadier
BEFORE INSERT OR UPDATE ON brigadier
FOR EACH ROW
EXECUTE FUNCTION check_single_brigadier();


-- ensure site has one supervisor at a time
CREATE OR REPLACE TRIGGER enforce_single_site_supervisor
BEFORE INSERT OR UPDATE ON site_supervisor
FOR EACH ROW
EXECUTE FUNCTION check_single_site_supervisor();


-- ensure department has one supervisor at a time
CREATE OR REPLACE TRIGGER enforce_single_department_supervisor
BEFORE INSERT OR UPDATE ON department_supervisor
FOR EACH ROW
EXECUTE FUNCTION check_single_department_supervisor();


-- ensure worker is assigneed to a single brigade at a time
CREATE OR REPLACE TRIGGER enforce_worker_single_brigade
BEFORE INSERT OR UPDATE ON brigade_worker
FOR EACH ROW
    EXECUTE FUNCTION check_worker_single_brigade();


-- ensure specialist is assigneed to a single site at a time
CREATE OR REPLACE TRIGGER enforce_specialist_single_site
BEFORE INSERT OR UPDATE ON site_specialist
FOR EACH ROW
    EXECUTE FUNCTION check_specialist_single_site();


-- ensure employee is not a brigadier and a worker at the same time
CREATE OR REPLACE TRIGGER enforce_brigade_worker_role_exclusivity
BEFORE INSERT OR UPDATE ON brigade_worker
FOR EACH ROW
    EXECUTE FUNCTION check_brigade_worker_no_brigadier();


-- ensure employee is not a brigadier and a worker at the same time
CREATE OR REPLACE TRIGGER enforce_brigadier_role_exclusivity
BEFORE INSERT OR UPDATE ON brigadier
FOR EACH ROW
    EXECUTE FUNCTION check_brigadier_no_brigade_worker();


-- ensure employee is not a site_supervisor and a site_specialist at the same time
CREATE OR REPLACE TRIGGER enforce_site_specialist_role_exclusivity
BEFORE INSERT OR UPDATE ON site_specialist
FOR EACH ROW
    EXECUTE FUNCTION check_site_specialist_no_site_supervisor();


-- ensure employee is not a site_supervisor and a site_specialist at the same time
CREATE OR REPLACE TRIGGER enforce_site_supervisor_role_exclusivity
BEFORE INSERT OR UPDATE ON site_supervisor
FOR EACH ROW
    EXECUTE FUNCTION check_site_supervisor_no_site_specialist();

-- ensure worker is not a brigadier of > 1 brigades at a time
CREATE TRIGGER enforce_worker_single_brigadier
BEFORE INSERT OR UPDATE ON brigadier
FOR EACH ROW
EXECUTE FUNCTION check_worker_single_brigadier();

-- ensure specialist is not a supervisor of > 1 sites at a time
CREATE TRIGGER enforce_specialist_single_site_supervisor
BEFORE INSERT OR UPDATE ON site_supervisor
FOR EACH ROW
EXECUTE FUNCTION check_specialist_single_site_supervisor();

-- ensure specialist is not a supervisor of > 1 departments at a time
CREATE TRIGGER enforce_specialist_single_department_supervisor
BEFORE INSERT OR UPDATE ON department_supervisor
FOR EACH ROW
EXECUTE FUNCTION check_specialist_single_department_supervisor();

-- ensure machine is not assigned to > 1 sites at the same time
CREATE TRIGGER enforce_machine_single_site_assignment
BEFORE INSERT OR UPDATE ON site_machine
FOR EACH ROW
EXECUTE FUNCTION check_machine_single_site_assignment();

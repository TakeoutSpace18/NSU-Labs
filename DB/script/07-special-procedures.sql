CREATE OR REPLACE FUNCTION create_object_with_schedule(
    p_object_name VARCHAR(100),
    p_object_type object_type,
    p_site_id BIGINT,
    p_planned_start_date DATE,
    p_planned_end_date DATE,
    p_actual_start_date DATE DEFAULT NULL,
    p_actual_end_date DATE DEFAULT NULL,
    
    -- Customer parameters (create new or use existing)
    p_customer_id BIGINT DEFAULT NULL,
    p_customer_name VARCHAR(100) DEFAULT NULL,
    p_customer_address VARCHAR(300) DEFAULT NULL,
    p_customer_phone VARCHAR(20) DEFAULT NULL,
    p_customer_email VARCHAR(100) DEFAULT NULL,
    
    -- Object type specific parameters
    p_type_params JSONB DEFAULT NULL,
    
    -- Work schedules array
    -- [{"work_type_id": 1, "brigade_id": 1, "planned_start": "2024-01-01", "planned_end": "2024-01-15"}, ...]
    p_work_schedules JSONB DEFAULT '[]'::jsonb
)
RETURNS TABLE(
    object_id BIGINT,
    customer_id BIGINT,
    work_schedule_ids BIGINT[]
) AS $$
DECLARE
    v_object_id BIGINT;
    v_customer_id BIGINT;
    v_work_schedule_id BIGINT;
    v_work_schedule_ids BIGINT[] := '{}';
    ws_record JSONB;
BEGIN
    BEGIN
        -- Handle customer creation or assignment
        IF p_customer_id IS NOT NULL THEN
            v_customer_id := p_customer_id;
        ELSE
            BEGIN
                INSERT INTO customer (name, address, phone, email)
                VALUES (p_customer_name, p_customer_address, p_customer_phone, p_customer_email)
                RETURNING customer.customer_id INTO v_customer_id;
            EXCEPTION WHEN OTHERS THEN
                RAISE EXCEPTION 'Failed to create customer: %', SQLERRM;
            END;
        END IF;
        
        -- Create main object
        BEGIN
            INSERT INTO object (name, type, site_id, customer_id, planned_start_date, planned_end_date, actual_start_date, actual_end_date)
            VALUES (p_object_name, p_object_type, p_site_id, v_customer_id, p_planned_start_date, p_planned_end_date, p_actual_start_date, p_actual_end_date)
            RETURNING object.object_id INTO v_object_id;
        EXCEPTION WHEN OTHERS THEN
            RAISE EXCEPTION 'Failed to create object: %', SQLERRM;
        END;
        
        -- Create type-specific object record
        IF p_type_params IS NOT NULL THEN
            BEGIN
                CASE p_object_type
                    WHEN 'bridge' THEN
                        INSERT INTO bridge_object (object_id, length_m, width_m)
                        VALUES (v_object_id, (p_type_params->>'length_m')::NUMERIC, (p_type_params->>'width_m')::NUMERIC);
                    
                    WHEN 'residential' THEN
                        INSERT INTO residential_object (object_id, floors_number, has_parking_lot)
                        VALUES (v_object_id, (p_type_params->>'floors_number')::INTEGER, (p_type_params->>'has_parking_lot')::BOOLEAN);
                    
                    WHEN 'road' THEN
                        INSERT INTO road_object (object_id, length_m)
                        VALUES (v_object_id, (p_type_params->>'length_m')::NUMERIC);
                    
                    WHEN 'hospital' THEN
                        INSERT INTO hospital_object (object_id, beds_number)
                        VALUES (v_object_id, (p_type_params->>'beds_number')::INTEGER);
                    
                    WHEN 'school' THEN
                        INSERT INTO school_object (object_id, classrooms_number)
                        VALUES (v_object_id, (p_type_params->>'classrooms_number')::INTEGER);
                END CASE;
            EXCEPTION WHEN OTHERS THEN
                RAISE EXCEPTION 'Failed to create type-specific object record: %', SQLERRM;
            END;
        END IF;
        
        -- Create work schedules
        FOR ws_record IN SELECT * FROM jsonb_array_elements(p_work_schedules)
        LOOP
            BEGIN
                INSERT INTO work_schedule (
                    object_id, 
                    work_type_id, 
                    brigade_id,
                    planned_start_date, 
                    planned_end_date,
                    actual_start_date,
                    actual_end_date
                )
                VALUES (
                    v_object_id,
                    (ws_record->>'work_type_id')::BIGINT,
                    CASE WHEN ws_record->>'brigade_id' != 'null' THEN (ws_record->>'brigade_id')::BIGINT ELSE NULL END,
                    (ws_record->>'planned_start_date')::DATE,
                    (ws_record->>'planned_end_date')::DATE,
                    CASE WHEN ws_record->>'actual_start_date' != 'null' THEN (ws_record->>'actual_start_date')::DATE ELSE NULL END,
                    CASE WHEN ws_record->>'actual_end_date' != 'null' THEN (ws_record->>'actual_end_date')::DATE ELSE NULL END
                )
                RETURNING work_schedule.work_schedule_id INTO v_work_schedule_id;
                
                v_work_schedule_ids := array_append(v_work_schedule_ids, v_work_schedule_id);
            EXCEPTION WHEN OTHERS THEN
                RAISE EXCEPTION 'Failed to create work schedule: %', SQLERRM;
            END;
        END LOOP;
        
        -- Return results
        RETURN QUERY SELECT v_object_id, v_customer_id, v_work_schedule_ids;
        
    EXCEPTION WHEN OTHERS THEN
        RAISE EXCEPTION 'Transaction failed: %', SQLERRM;
    END;
END;
$$ LANGUAGE plpgsql;

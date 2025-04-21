BEGIN;

CREATE FUNCTION is_valid_email(email TEXT)
RETURNS BOOLEAN AS $$
BEGIN
    -- String matches regular expression, case-insensitively
    RETURN email ~* '^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$';
END;
$$ LANGUAGE plpgsql IMMUTABLE;

CREATE FUNCTION is_valid_phone(phone TEXT)
RETURNS BOOLEAN AS $$
BEGIN
    -- String matches regular expression, case sensitively
    RETURN phone ~ '^(\+)?[0-9]{10,15}$';
END;
$$ LANGUAGE plpgsql IMMUTABLE;

CREATE TABLE department (
    department_id BIGSERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL UNIQUE
);

CREATE TABLE site (
    site_id BIGSERIAL PRIMARY KEY,
    department_id INTEGER NOT NULL REFERENCES department(department_id) ON DELETE CASCADE,
    address VARCHAR(300) NOT NULL UNIQUE
);

CREATE TABLE customer (
    customer_id BIGSERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    address VARCHAR(300),
    phone VARCHAR(20) CHECK (is_valid_phone(phone)),
    email VARCHAR(100) CHECK (is_valid_email(email))
);

CREATE TYPE object_type AS ENUM (
    'bridge',
    'residential',
    'road',
    'hospital',
    'school'
);

CREATE TABLE object (
    object_id BIGSERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    type object_type,
    site_id INTEGER NOT NULL REFERENCES site(site_id),
    customer_id INTEGER NOT NULL REFERENCES customer(customer_id),
    planned_start_date DATE,
    planned_end_date DATE,
    actual_start_date DATE,
    actual_end_date DATE

    CHECK (planned_start_date <= planned_end_date),
    CHECK (actual_start_date <= actual_end_date)
);

CREATE TABLE bridge_object (
    object_id BIGINT PRIMARY KEY REFERENCES object(object_id) ON DELETE CASCADE,
    length_m NUMERIC(10,2) CHECK (length_m > 0),
    width_m NUMERIC(10,2) CHECK (width_m > 0)
);

CREATE TABLE residential_object (
    object_id BIGINT PRIMARY KEY REFERENCES object(object_id) ON DELETE CASCADE,
    floors_number INTEGER CHECK (floors_number > 0),
    has_parking_lot BOOLEAN
);

CREATE TABLE road_object (
    object_id BIGINT PRIMARY KEY REFERENCES object(object_id) ON DELETE CASCADE,
    length_m NUMERIC(15,2) CHECK (length_m > 0)
);

CREATE TABLE hospital_object (
    object_id BIGINT PRIMARY KEY REFERENCES object(object_id) ON DELETE CASCADE,
    beds_number INTEGER CHECK (beds_number > 0)
);

CREATE TABLE school_object (
    object_id BIGINT PRIMARY KEY REFERENCES object(object_id) ON DELETE CASCADE,
    classrooms_number INTEGER CHECK (classrooms_number > 0)

);

CREATE TABLE employee (
    employee_id BIGSERIAL PRIMARY KEY,
    name VARCHAR(50) NOT NULL,
    surname VARCHAR(50) NOT NULL,
    middlename VARCHAR(50),
    phone VARCHAR(20) CHECK (is_valid_phone(phone)),
    email VARCHAR(100) CHECK (is_valid_email(email)),
    salary MONEY
);

CREATE TABLE grade (
    grade_id BIGSERIAL PRIMARY KEY,
    grade VARCHAR(50) NOT NULL UNIQUE,
    bonus_payment MONEY
);

CREATE TABLE worker_type (
    worker_type_id BIGSERIAL PRIMARY KEY,
    type VARCHAR(50) NOT NULL UNIQUE,
    description TEXT
);

CREATE TABLE worker (
    employee_id BIGINT PRIMARY KEY REFERENCES employee(employee_id) ON DELETE CASCADE,
    worker_type_id BIGINT NOT NULL REFERENCES worker_type(worker_type_id) ON DELETE RESTRICT
);

CREATE TABLE specialist_type (
    specialist_type_id BIGSERIAL PRIMARY KEY,
    type VARCHAR(50) NOT NULL UNIQUE,
    description TEXT
);

CREATE TABLE specialist (
    employee_id BIGINT PRIMARY KEY REFERENCES employee(employee_id) ON DELETE CASCADE,
    specialist_type_id BIGINT NOT NULL REFERENCES specialist_type(specialist_type_id) ON DELETE RESTRICT
);

CREATE TABLE brigade (
    brigade_id BIGSERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL UNIQUE
);

CREATE TABLE work_type (
    work_type_id BIGSERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL UNIQUE,
    description TEXT
);

CREATE TABLE work_schedule (
    work_schedule_id BIGSERIAL PRIMARY KEY,
    object_id BIGINT NOT NULL REFERENCES object(object_id) ON DELETE CASCADE,
    work_type_id BIGINT NOT NULL REFERENCES work_type(work_type_id) ON DELETE RESTRICT,
    brigade_id BIGINT REFERENCES brigade(brigade_id) ON DELETE RESTRICT,
    planned_start_date DATE,
    planned_end_date DATE,
    actual_start_date DATE,
    actual_end_date DATE,

    CHECK (planned_start_date <= planned_end_date),
    CHECK (actual_start_date <= actual_end_date)
);

CREATE TABLE material (
    material_id BIGSERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL UNIQUE,
    description TEXT,
    unit VARCHAR(20) NOT NULL,
    price MONEY
);

CREATE TABLE material_supply (
    material_supply_id BIGSERIAL PRIMARY KEY,
    material_id BIGINT NOT NULL REFERENCES material(material_id) ON DELETE RESTRICT,
    work_schedule_id BIGINT NOT NULL REFERENCES work_schedule(work_schedule_id) ON DELETE CASCADE,
    planned_quantity NUMERIC(10,2) CHECK (planned_quantity > 0),
    actual_quantity NUMERIC(10,2) CHECK (actual_quantity > 0)
);

CREATE TABLE department_supervisor (
    department_supervisor_id BIGSERIAL PRIMARY KEY,
    department_id BIGINT NOT NULL REFERENCES department(department_id) ON DELETE CASCADE,
    specialist_id BIGINT NOT NULL REFERENCES specialist(employee_id) ON DELETE CASCADE,
    start_date DATE NOT NULL DEFAULT CURRENT_DATE,
    end_date DATE,

    CHECK (start_date <= end_date)
);

CREATE TABLE site_supervisor (
    site_supervisor_id BIGSERIAL PRIMARY KEY,
    site_id BIGINT NOT NULL REFERENCES site(site_id) ON DELETE CASCADE,
    specialist_id BIGINT NOT NULL REFERENCES specialist(employee_id) ON DELETE CASCADE,
    start_date DATE NOT NULL DEFAULT CURRENT_DATE,
    end_date DATE,

    CHECK (start_date <= end_date)
);

CREATE TABLE site_specialist (
    site_specialist_id BIGSERIAL PRIMARY KEY,
    site_id BIGINT NOT NULL REFERENCES site(site_id) ON DELETE CASCADE,
    specialist_id BIGINT NOT NULL REFERENCES specialist(employee_id) ON DELETE CASCADE,
    start_date DATE NOT NULL DEFAULT CURRENT_DATE,
    end_date DATE,

    CHECK (start_date <= end_date)
);

CREATE TABLE brigade_worker (
    brigade_worker_id BIGSERIAL PRIMARY KEY,
    brigade_id BIGINT NOT NULL REFERENCES brigade(brigade_id) ON DELETE CASCADE,
    worker_id BIGINT NOT NULL REFERENCES worker(employee_id) ON DELETE CASCADE,
    start_date DATE NOT NULL DEFAULT CURRENT_DATE,
    end_date DATE

    CHECK (start_date <= end_date)
);

CREATE TABLE brigadier (
    brigadier_id BIGSERIAL PRIMARY KEY,
    brigade_id BIGINT NOT NULL REFERENCES brigade(brigade_id) ON DELETE CASCADE,
    worker_id BIGINT NOT NULL REFERENCES worker(employee_id) ON DELETE CASCADE,
    start_date DATE NOT NULL DEFAULT CURRENT_DATE,
    end_date DATE,
    
    CHECK (start_date < end_date)
);

CREATE TABLE machine_type (
    machine_type_id BIGSERIAL PRIMARY KEY,
    type VARCHAR(50) NOT NULL UNIQUE,
    description TEXT
);

CREATE TABLE machine_model (
    machine_model_id BIGSERIAL PRIMARY KEY,
    model VARCHAR(100) NOT NULL UNIQUE,
    machine_type_id BIGINT NOT NULL REFERENCES machine_type(machine_type_id) ON DELETE RESTRICT
);

CREATE TABLE machine (
    machine_id BIGSERIAL PRIMARY KEY,
    machine_model_id BIGINT NOT NULL REFERENCES machine_model(machine_model_id),
    department_id BIGINT NOT NULL REFERENCES department(department_id),
    production_year INTEGER CHECK (production_year <= EXTRACT(YEAR FROM CURRENT_DATE))
);

CREATE TABLE site_machine (
    site_machine_id BIGSERIAL PRIMARY KEY,
    site_id BIGINT NOT NULL REFERENCES site(site_id) ON DELETE CASCADE,
    machine_id BIGINT NOT NULL REFERENCES machine(machine_id) ON DELETE RESTRICT,
    start_date DATE NOT NULL DEFAULT CURRENT_DATE,
    end_date DATE,

    CHECK (start_date <= end_date)
);
COMMIT;

BEGIN;

-- Departments
INSERT INTO department (name) VALUES
('Engineering'),
('Logistics'),
('Architecture');

-- Sites
INSERT INTO site (department_id, address) VALUES
(1, '123 Build St, Construct City'),
(2, '456 Project Ave, Construct City');

-- Customers
INSERT INTO customer (name, address, phone, email) VALUES
('Urban Planning Co.', '789 Capital Blvd, Capital City', '+12345678901', 'contact@urbanplan.co'),
('HealthInfra Ltd.', '321 Health Dr, Medic Town', '+19876543210', 'info@healthinfra.com');

-- Objects
INSERT INTO object (name, type, site_id, customer_id, planned_start_date, planned_end_date, actual_start_date, actual_end_date) VALUES
('Central Bridge', 'bridge', 1, 1, '2024-01-01', '2024-12-31', '2024-01-15', NULL),
('Sunrise Apartments', 'residential', 2, 1, '2024-03-01', '2025-03-01', '2024-03-05', NULL),
('Mediclinic Hospital', 'hospital', 1, 2, '2024-05-01', '2026-05-01', NULL, NULL);

-- Subtype objects
INSERT INTO bridge_object (object_id, length_m, width_m) VALUES (1, 120.5, 20.3);
INSERT INTO residential_object (object_id, floors_number, has_parking_lot) VALUES (2, 15, TRUE);
INSERT INTO hospital_object (object_id, beds_number) VALUES (3, 300);

-- Grades
INSERT INTO grade (grade, bonus_payment) VALUES
('Junior', 100.0),
('Middle', 200.0),
('Senior', 300.0);

-- Worker/Specialist Types
INSERT INTO worker_type (type, description) VALUES
('Electrician', 'Handles electrical systems'),
('Plumber', 'Handles water and piping');

INSERT INTO specialist_type (type, description) VALUES
('Engineer', 'Designs and oversees construction'),
('Architect', 'Designs blueprints and structure');

-- Employees
INSERT INTO employee (name, surname, middlename, email, salary) VALUES
('John', 'Doe', 'E', 'john.doe@buildnova.com', 3000),
('Alice', 'Smith', 'B', 'alice.smith@buildnova.com', 4000),
('Bob', 'Brown', 'C', 'bob.brown@buildnova.com', 5000),
('Carol', 'White', 'D', 'carol.white@buildnova.com', 3500);

-- Workers and Specialists
INSERT INTO worker (employee_id, worker_type_id) VALUES
(1, 1),
(4, 2);

INSERT INTO specialist (employee_id, specialist_type_id) VALUES
(2, 1),
(3, 2);

-- Brigade
INSERT INTO brigade (name) VALUES ('Alpha Team');

-- Brigade Members
INSERT INTO brigade_worker (brigade_id, worker_id, start_date, end_date) VALUES
(1, 1, '2024-01-01', NULL);

INSERT INTO brigadier (brigade_id, worker_id, start_date, end_date) VALUES
(1, 4, '2024-01-01', NULL);

-- Work Types
INSERT INTO work_type (name, description) VALUES
('Foundation', 'Laying building foundations'),
('Framing', 'Constructing structural frames');

-- Work Schedule
INSERT INTO work_schedule (object_id, work_type_id, brigade_id, planned_start_date, planned_end_date, actual_start_date, actual_end_date) VALUES
(2, 1, 1, '2024-03-10', '2024-04-10', '2024-03-12', NULL);

-- Materials
INSERT INTO material (name, description, unit, price) VALUES
('Concrete', 'High strength', 'kg', 100.0),
('Rebar', 'Steel reinforcement', 'pieces', 50.0);

-- Material Supply
INSERT INTO material_supply (material_id, work_schedule_id, planned_quantity, actual_quantity) VALUES
(1, 1, 1000, 800),
(2, 1, 500, 400);

-- Machines
INSERT INTO machine_type (type, description) VALUES
('Excavator', 'Heavy digging machine'),
('Crane', 'Lifting machine');

INSERT INTO machine_model (model, machine_type_id) VALUES
('CAT320', 1),
('Liebherr LTM', 2);

INSERT INTO machine (machine_model_id, department_id, production_year) VALUES
(1, 1, 2020),
(2, 2, 2019);

INSERT INTO site_machine (site_id, machine_id, start_date, end_date) VALUES
(1, 1, '2024-01-10', NULL),
(2, 2, '2024-03-01', NULL);

COMMIT;

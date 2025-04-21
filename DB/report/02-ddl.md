# Отчет по DDL и ограничениям БД

[DDL Script](scripts/ddl.sql)

## Functions

### `is_valid_email(email TEXT)`
- **Returns:** BOOLEAN
- **Description:** Validates email format using regex pattern
- **Pattern:** `^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$`

### `is_valid_phone(phone TEXT)`
- **Returns:** BOOLEAN
- **Description:** Validates phone number format using regex pattern
- **Pattern:** `^(\+)?[0-9]{10,15}$`

## Tables

### department
| Column Name      | Type         | Constraints                     |
|------------------|--------------|---------------------------------|
| department_id    | BIGSERIAL    | PRIMARY KEY                     |
| name             | VARCHAR(100) | NOT NULL, UNIQUE                |

### site
| Column Name      | Type         | Constraints                     |
|------------------|--------------|---------------------------------|
| site_id          | BIGSERIAL    | PRIMARY KEY                     |
| department_id    | INTEGER      | NOT NULL, REFERENCES department(department_id) ON DELETE CASCADE |
| address          | VARCHAR(300) | NOT NULL, UNIQUE                |

### customer
| Column Name      | Type         | Constraints                     |
|------------------|--------------|---------------------------------|
| customer_id      | BIGSERIAL    | PRIMARY KEY                     |
| name             | VARCHAR(100) | NOT NULL                        |
| address          | VARCHAR(300) |                                 |
| phone            | VARCHAR(20)  | CHECK (is_valid_phone(phone))   |
| email            | VARCHAR(100) | CHECK (is_valid_email(email))   |

### object_type (ENUM)
- bridge
- residential
- road
- hospital
- school

### object
| Column Name         | Type         | Constraints                     |
|---------------------|--------------|---------------------------------|
| object_id           | BIGSERIAL    | PRIMARY KEY                     |
| name                | VARCHAR(100) | NOT NULL                        |
| type                | object_type  |                                 |
| site_id             | INTEGER      | NOT NULL, REFERENCES site(site_id) |
| customer_id         | INTEGER      | NOT NULL, REFERENCES customer(customer_id) |
| planned_start_date  | DATE         |                                 |
| planned_end_date    | DATE         |                                 |
| actual_start_date   | DATE         |                                 |
| actual_end_date     | DATE         |                                 |

**CHECK constraints:**
- `planned_start_date <= planned_end_date`
- `actual_start_date <= actual_end_date`

### bridge_object
| Column Name | Type         | Constraints                     |
|-------------|--------------|---------------------------------|
| object_id   | BIGINT       | PRIMARY KEY, REFERENCES object(object_id) ON DELETE CASCADE |
| length_m    | NUMERIC(10,2)| CHECK (length_m > 0)            |
| width_m     | NUMERIC(10,2)| CHECK (width_m > 0)             |

### residential_object
| Column Name     | Type    | Constraints                     |
|-----------------|---------|---------------------------------|
| object_id       | BIGINT  | PRIMARY KEY, REFERENCES object(object_id) ON DELETE CASCADE |
| floors_number   | INTEGER | CHECK (floors_number > 0)       |
| has_parking_lot | BOOLEAN |                                 |

### road_object
| Column Name | Type         | Constraints                     |
|-------------|--------------|---------------------------------|
| object_id   | BIGINT       | PRIMARY KEY, REFERENCES object(object_id) ON DELETE CASCADE |
| length_m    | NUMERIC(15,2)| CHECK (length_m > 0)            |

### hospital_object
| Column Name | Type    | Constraints                     |
|-------------|---------|---------------------------------|
| object_id   | BIGINT  | PRIMARY KEY, REFERENCES object(object_id) ON DELETE CASCADE |
| beds_number | INTEGER | CHECK (beds_number > 0)         |

### school_object
| Column Name      | Type    | Constraints                     |
|------------------|---------|---------------------------------|
| object_id        | BIGINT  | PRIMARY KEY, REFERENCES object(object_id) ON DELETE CASCADE |
| classrooms_number| INTEGER | CHECK (classrooms_number > 0)   |

### employee
| Column Name | Type         | Constraints                     |
|-------------|--------------|---------------------------------|
| employee_id | BIGSERIAL    | PRIMARY KEY                     |
| name        | VARCHAR(50)  | NOT NULL                        |
| surname     | VARCHAR(50)  | NOT NULL                        |
| middlename  | VARCHAR(50)  |                                 |
| phone       | VARCHAR(20)  | CHECK (is_valid_phone(phone))   |
| email       | VARCHAR(100) | CHECK (is_valid_email(email))   |
| salary      | MONEY        |                                 |

### grade
| Column Name   | Type         | Constraints      |
|---------------|--------------|------------------|
| grade_id      | BIGSERIAL    | PRIMARY KEY      |
| grade         | VARCHAR(50)  | NOT NULL, UNIQUE |
| bonus_payment | MONEY        |                  |

### worker_type
| Column Name   | Type         | Constraints      |
|---------------|--------------|------------------|
| worker_type_id| BIGSERIAL    | PRIMARY KEY      |
| type          | VARCHAR(50)  | NOT NULL, UNIQUE |
| description   | TEXT         |                  |

### worker
| Column Name   | Type    | Constraints                     |
|---------------|---------|---------------------------------|
| employee_id   | BIGINT  | PRIMARY KEY, REFERENCES employee(employee_id) ON DELETE CASCADE |
| worker_type_id| BIGINT  | NOT NULL, REFERENCES worker_type(worker_type_id) ON DELETE RESTRICT |

### specialist_type
| Column Name       | Type         | Constraints      |
|-------------------|--------------|------------------|
| specialist_type_id| BIGSERIAL    | PRIMARY KEY      |
| type              | VARCHAR(50)  | NOT NULL, UNIQUE |
| description       | TEXT         |                  |

### specialist
| Column Name       | Type    | Constraints                     |
|-------------------|---------|---------------------------------|
| employee_id       | BIGINT  | PRIMARY KEY, REFERENCES employee(employee_id) ON DELETE CASCADE |
| specialist_type_id| BIGINT  | NOT NULL, REFERENCES specialist_type(specialist_type_id) ON DELETE RESTRICT |

### brigade
| Column Name | Type         | Constraints      |
|-------------|--------------|------------------|
| brigade_id  | BIGSERIAL    | PRIMARY KEY      |
| name        | VARCHAR(100) | NOT NULL, UNIQUE |

### work_type
| Column Name | Type         | Constraints      |
|-------------|--------------|------------------|
| work_type_id| BIGSERIAL    | PRIMARY KEY      |
| name        | VARCHAR(100) | NOT NULL, UNIQUE |
| description | TEXT         |                  |

### work_schedule
| Column Name         | Type    | Constraints                     |
|---------------------|---------|---------------------------------|
| work_schedule_id    | BIGSERIAL| PRIMARY KEY                    |
| object_id           | BIGINT  | NOT NULL, REFERENCES object(object_id) ON DELETE CASCADE |
| work_type_id        | BIGINT  | NOT NULL, REFERENCES work_type(work_type_id) ON DELETE RESTRICT |
| brigade_id          | BIGINT  | REFERENCES brigade(brigade_id) ON DELETE RESTRICT |
| planned_start_date  | DATE    |                                 |
| planned_end_date    | DATE    |                                 |
| actual_start_date   | DATE    |                                 |
| actual_end_date     | DATE    |                                 |

**CHECK constraints:**
- `planned_start_date <= planned_end_date`
- `actual_start_date <= actual_end_date`

### material
| Column Name | Type         | Constraints      |
|-------------|--------------|------------------|
| material_id | BIGSERIAL    | PRIMARY KEY      |
| name        | VARCHAR(100) | NOT NULL, UNIQUE |
| description | TEXT         |                  |
| unit        | VARCHAR(20)  | NOT NULL         |
| price       | MONEY        |                  |

### material_supply
| Column Name       | Type         | Constraints                     |
|-------------------|--------------|---------------------------------|
| material_supply_id| BIGSERIAL    | PRIMARY KEY                     |
| material_id       | BIGINT       | NOT NULL, REFERENCES material(material_id) ON DELETE RESTRICT |
| work_schedule_id  | BIGINT       | NOT NULL, REFERENCES work_schedule(work_schedule_id) ON DELETE CASCADE |
| planned_quantity  | NUMERIC(10,2)| CHECK (planned_quantity > 0)    |
| actual_quantity   | NUMERIC(10,2)| CHECK (actual_quantity > 0)     |

### department_supervisor
| Column Name           | Type    | Constraints                     |
|-----------------------|---------|---------------------------------|
| department_supervisor_id| BIGSERIAL| PRIMARY KEY                   |
| department_id         | BIGINT  | NOT NULL, REFERENCES department(department_id) ON DELETE CASCADE |
| specialist_id         | BIGINT  | NOT NULL, REFERENCES specialist(employee_id) ON DELETE CASCADE |
| start_date           | DATE    | NOT NULL, DEFAULT CURRENT_DATE  |
| end_date             | DATE    |                                 |

**CHECK constraints:**
- `start_date <= end_date`

### site_supervisor
| Column Name      | Type    | Constraints                     |
|------------------|---------|---------------------------------|
| site_supervisor_id| BIGSERIAL| PRIMARY KEY                    |
| site_id          | BIGINT  | NOT NULL, REFERENCES site(site_id) ON DELETE CASCADE |
| specialist_id    | BIGINT  | NOT NULL, REFERENCES specialist(employee_id) ON DELETE CASCADE |
| start_date       | DATE    | NOT NULL, DEFAULT CURRENT_DATE  |
| end_date         | DATE    |                                 |

**CHECK constraints:**
- `start_date <= end_date`

### site_specialist
| Column Name      | Type    | Constraints                     |
|------------------|---------|---------------------------------|
| site_specialist_id| BIGSERIAL| PRIMARY KEY                    |
| site_id          | BIGINT  | NOT NULL, REFERENCES site(site_id) ON DELETE CASCADE |
| specialist_id    | BIGINT  | NOT NULL, REFERENCES specialist(employee_id) ON DELETE CASCADE |
| start_date       | DATE    | NOT NULL, DEFAULT CURRENT_DATE  |
| end_date         | DATE    |                                 |

**CHECK constraints:**
- `start_date <= end_date`

### brigade_worker
| Column Name      | Type    | Constraints                     |
|------------------|---------|---------------------------------|
| brigade_worker_id| BIGSERIAL| PRIMARY KEY                    |
| brigade_id       | BIGINT  | NOT NULL, REFERENCES brigade(brigade_id) ON DELETE CASCADE |
| worker_id        | BIGINT  | NOT NULL, REFERENCES worker(employee_id) ON DELETE CASCADE |
| start_date       | DATE    | NOT NULL, DEFAULT CURRENT_DATE  |
| end_date         | DATE    |                                 |

**CHECK constraints:**
- `start_date <= end_date`

### brigadier
| Column Name  | Type    | Constraints                     |
|--------------|---------|---------------------------------|
| brigadier_id | BIGSERIAL| PRIMARY KEY                    |
| brigade_id   | BIGINT  | NOT NULL, REFERENCES brigade(brigade_id) ON DELETE CASCADE |
| worker_id    | BIGINT  | NOT NULL, REFERENCES worker(employee_id) ON DELETE CASCADE |
| start_date   | DATE    | NOT NULL, DEFAULT CURRENT_DATE  |
| end_date     | DATE    |                                 |

**CHECK constraints:**
- `start_date < end_date`

### machine_type
| Column Name   | Type         | Constraints      |
|---------------|--------------|------------------|
| machine_type_id| BIGSERIAL    | PRIMARY KEY      |
| type          | VARCHAR(50)  | NOT NULL, UNIQUE |
| description   | TEXT         |                  |

### machine_model
| Column Name   | Type         | Constraints                     |
|---------------|--------------|---------------------------------|
| machine_model_id| BIGSERIAL   | PRIMARY KEY                     |
| model         | VARCHAR(100) | NOT NULL, UNIQUE                |
| machine_type_id| BIGINT      | NOT NULL, REFERENCES machine_type(machine_type_id) ON DELETE RESTRICT |

### machine
| Column Name   | Type    | Constraints                     |
|---------------|---------|---------------------------------|
| machine_id    | BIGSERIAL| PRIMARY KEY                    |
| machine_model_id| BIGINT | NOT NULL, REFERENCES machine_model(machine_model_id) |
| department_id | BIGINT  | NOT NULL, REFERENCES department(department_id) |
| production_year| INTEGER | CHECK (production_year <= EXTRACT(YEAR FROM CURRENT_DATE)) |

### site_machine
| Column Name  | Type    | Constraints                     |
|--------------|---------|---------------------------------|
| site_machine_id| BIGSERIAL| PRIMARY KEY                   |
| site_id      | BIGINT  | NOT NULL, REFERENCES site(site_id) ON DELETE CASCADE |
| machine_id   | BIGINT  | NOT NULL, REFERENCES machine(machine_id) ON DELETE RESTRICT |
| start_date   | DATE    | NOT NULL, DEFAULT CURRENT_DATE  |
| end_date     | DATE    |                                 |

**CHECK constraints:**
- `start_date <= end_date`

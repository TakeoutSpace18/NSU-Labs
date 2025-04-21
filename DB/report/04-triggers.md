# Отчет по триггерам базы данных

## Триггеры для проверки временных ограничений

### `enforce_work_schedule_date_constraints`
- **Таблица**: `work_schedule`
- **Функция**: `check_work_schedule_dates()`
- **Описание**:
  - Планируемая дата начала работы не может быть раньше планируемой даты начала объекта
  - Планируемая дата окончания работы не может быть позже планируемой даты окончания объекта
  - Фактическая дата начала работы не может быть раньше фактической даты начала объекта
  - Фактическая дата окончания работы не может быть позже фактической даты окончания объекта

### `enforce_object_date_constraints`
- **Таблица**: `object`
- **Функция**: `check_object_date_constraints()`
- **Описание**:
  - При обновлении дат объекта проверяет, что все связанные работы остаются в пределах новых дат объекта

## Триггеры для обеспечения уникальности назначений

### `enforce_single_brigadier`
- **Таблица**: `brigadier`
- **Функция**: `check_single_brigadier()`
- **Описание**:
  - В бригаде может быть только один активный бригадир в определённый момент времени

### `enforce_single_site_supervisor`
- **Таблица**: `site_supervisor`
- **Функция**: `check_single_site_supervisor()`
- **Описание**:
  - На строительной площадке может быть только один активный руководитель в определённый момент времени

### `enforce_single_department_supervisor`
- **Таблица**: `department_supervisor`
- **Функция**: `check_single_department_supervisor()`
- **Описание**:
  - В отделе может быть только один активный руководитель в определённый момент времени

### `enforce_worker_single_brigade`
- **Таблица**: `brigade_worker`
- **Функция**: `check_worker_single_brigade()`
- **Описание**:
  - Рабочий может быть назначен только в одну бригаду в определённый момент времени

### `enforce_specialist_single_site`
- **Таблица**: `site_specialist`
- **Функция**: `check_specialist_single_site()`
- **Описание**:
  - Специалист может быть назначен только на одну площадку в определённый момент времени

### `enforce_machine_single_site_assignment`
- **Таблица**: `site_machine`
- **Функция**: `check_machine_single_site_assignment()`
- **Описание**:
  - Машина может быть назначена только на одну площадку в определённый момент времени

## Триггеры для обеспечения исключительности ролей

### `enforce_brigade_worker_role_exclusivity`
- **Таблица**: `brigade_worker`
- **Функция**: `check_brigade_worker_no_brigadier()`
- **Описание**:
  - Рабочий не может быть одновременно бригадиром и рядовым рабочим

### `enforce_brigadier_role_exclusivity`
- **Таблица**: `brigadier`
- **Функция**: `check_brigadier_no_brigade_worker()`
- **Описание**:
  - Бригадир не может быть одновременно рядовым рабочим в бригаде

### `enforce_site_specialist_role_exclusivity`
- **Таблица**: `site_specialist`
- **Функция**: `check_site_specialist_no_site_supervisor()`
- **Описание**:
  - Специалист площадки не может быть одновременно руководителем площадки

### `enforce_site_supervisor_role_exclusivity`
- **Таблица**: `site_supervisor`
- **Функция**: `check_site_supervisor_no_site_specialist()`
- **Описание**:
  - Руководитель площадки не может быть одновременно рядовым специалистом на площадке

## Триггеры для проверки уникальности руководящих должностей

### `enforce_worker_single_brigadier`
- **Таблица**: `brigadier`
- **Функция**: `check_worker_single_brigadier()`
- **Описание**:
  - Рабочий не может быть бригадиром более чем одной бригады одновременно

### `enforce_specialist_single_site_supervisor`
- **Таблица**: `site_supervisor`
- **Функция**: `check_specialist_single_site_supervisor()`
- **Описание**:
  - Специалист не может быть руководителем более чем одной площадки одновременно

### `enforce_specialist_single_department_supervisor`
- **Таблица**: `department_supervisor`
- **Функция**: `check_specialist_single_department_supervisor()`
- **Описание**:
  - Специалист не может быть руководителем более чем одного отдела одновременно

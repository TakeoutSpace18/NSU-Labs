## Роли пользователей приложения

[Roles script](script/03-roles.sql)

**Директор компании** - администратор системы, имеет доступ ко всем основным таблицами системы и может в случае тех или иных обстоятельств менять данные в них. Занимается поиском и общением с заказчиками, закупкой строительной техники фирмы. Также назначает начальников строительных управлений.\
**Менеджер по персоналу** - занимается наймом сотрудников и управляет данными об их квалификации.\
**Руководитель проекта** - занимается планированием строительного процесса, назначает бригады на выполнение работ, технику на участки, поставки материалов и начальников участков.

### Права доступа к сущностям
**C** - Create, **R** - Read, **U** - Update, **D** - Delete
| №  | Сущность                | Директор компании | Менеджер по персоналу | Руководитель проекта |
|----|-------------------------|-------------------|-----------------------|----------------------|
|  1 | department              | CRUD              | R                  | R                 |
|  2 | site                    | CRUD              | R                  | CRUD                 |
|  3 | object                  | CRUD              | R                  | R                 |
|  4 | object_type             | CRUD              | R                  | R                 |
|  5 | bridge_object           | CRUD              | R                  | R                 |
|  6 | residential_object      | CRUD              | R                  | R                 |
|  7 | road_object             | CRUD              | R                  | R                 |
|  8 | hospital_object         | CRUD              | R                  | R                 |
|  9 | school_object           | CRUD              | R                  | R                 |
| 10 | customer                | CRUD              | R                  | R                 |
| 11 | employee                | CRUD              | CRUD               | R                 |
| 12 | worker                  | CRUD              | CRUD               | R                 |
| 13 | worker_type             | CRUD              | CRUD               | R                 |
| 21 | specialist              | CRUD              | CRUD               | R                 |
| 22 | specialist_type         | CRUD              | CRUD               | R                 |
| 26 | brigade                 | CRUD              | R                  | CRUD                 |
| 27 | work_schedule           | CRUD              | R                  | CRUD                 |
| 28 | work_type               | CRUD              | R                  | CRUD                 |
| 29 | material_supply         | CRUD              | R                  | CRUD                 |
| 31 | department_supervisor   | CRUD              | R                  | R                 |
| 32 | site_supervisor         | CRUD              | R                  | CRUD                 |
| 33 | site_specialist         | CRUD              | R                  | CRUD                 |
| 34 | brigade_worker          | CRUD              | R                  | CRUD                 |
| 35 | brigadier               | CRUD              | R                  | CRUD                 |
| 36 | site_machine            | CRUD              | R                  | CRUD                 |
| 37 | machine                 | CRUD              | R                  | R                 |
| 38 | machine_model           | CRUD              | R                  | R                 |
| 39 | machine_type            | CRUD              | R                  | R                 |

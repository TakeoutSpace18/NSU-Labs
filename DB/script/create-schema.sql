\c cfdb
BEGIN;
\i /script/01-ddl.sql
\i /script/02-triggers.sql
\i /script/04-inserts.sql
\i /script/05-views.sql
\i /script/07-special-procedures.sql
\i /script/03-roles.sql
COMMIT;

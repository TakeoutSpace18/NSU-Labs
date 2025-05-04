BEGIN;
CREATE DATABASE cfdb;
\c cfdb
\ir 01-ddl.sql
\ir 02-triggers.sql
\ir 03-roles.sql
\ir 04-inserts.sql
\ir 04-views.sql
COMMIT;

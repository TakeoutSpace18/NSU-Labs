SELECT 'CREATE DATABASE dining_philosophers'
    WHERE NOT EXISTS (SELECT FROM pg_database WHERE datname = 'dining_philosophers')\gexec

\c dining_philosophers;

GRANT ALL PRIVILEGES ON DATABASE dining_philosophers TO postgres;
CREATE USER chatapp PASSWORD '12345';
CREATE DATABASE chatdb OWNER chatapp;

CREATE EXTENSION pgcrypto;

CREATE TABLE users (
    user_id SERIAL PRIMARY KEY,
    name VARCHAR(50) NOT NULL UNIQUE,
    password_hash VARCHAR(256) NOT NULL
);

CREATE TABLE messages (
    user_id integer REFERENCES users(user_id),
    timestamp timestamp DEFAULT now(),
    message_text text
);
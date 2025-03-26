CREATE TABLE users (
    id INT PRIMARY KEY,
    name STRING,
    balance INT,
    country STRING
) PARTITION BY LIST (country);

CREATE TABLE users_eu PARTITION OF users FOR
VALUES
    IN ('Germany', 'France', 'Italy');

CREATE TABLE users_us PARTITION OF users FOR
VALUES
    IN ('USA', 'Canada');

CREATE TABLE users_asia PARTITION OF users FOR
VALUES
    IN ('China', 'Japan', 'Korea');
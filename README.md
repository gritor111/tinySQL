# tinySQL
tinySQL is a simple SQL engine written in C++17. It uses CMake for building and GoogleTest for testing.

## Building the project

First, navigate to the root directory of the project and run:
```bash
cmake -S . -B build
```

Second, compile the project:
```bash
cmake --build build
```

## Running tests
From the root directory of the project run:
```bash
cd build
ctest
```


## Supported SQL syntax
Example:
```SQL
CREATE TABLE users (id INTEGER, name TEXT, age INTEGER) # supports TEXT and INTEGER type
INSERT INTO users VALUES (1, 'first', 80)
SELECT * FROM users WHERE id = 13 AND name = 'test' OR age > 70 # multi condition support
DELETE FROM users WHERE age = 80
DROP TABLE users
```

### Meta commands
```
.tables - shows how many tables are in the database
.quit - exists the REPL
```
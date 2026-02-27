#pragma once
#include <string>
#include <vector>
#include "Table.h" 

enum class StatementType {
    CREATE_TABLE,
    INSERT,
    SELECT,
};

// Base struct for all statements (Polymorphism)
struct SQLStatement {
    StatementType type;
    virtual ~SQLStatement() = default;
};

// CREATE TABLE users (id INTEGER, name TEXT, balance FLOAT);
struct CreateStatement : public SQLStatement {
    CreateStatement() { type = StatementType::CREATE_TABLE; }

    std::string tableName;
    std::vector<Table::Column> columns;
};

// INSERT INTO users VALUES (1, 'Alice', 4500.50);
struct InsertStatement : public SQLStatement {
    InsertStatement() { type = StatementType::INSERT; }

    std::string tableName;
    std::vector<std::string> values;
};

// SELECT id, name FROM users WHERE balance > 1000;
struct SelectStatement : public SQLStatement {
    SelectStatement() { type = StatementType::SELECT; }

    std::vector<Table::Column> columns;
    std::string tableName;

    // WHERE
    std::string filterColumn;
    std::string filterOperator;
    std::string filterValue;
};
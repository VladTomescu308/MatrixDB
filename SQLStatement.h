#pragma once
#include <string>
#include <vector>
#include "Table.h" 

enum class StatementType {
    CREATE_TABLE,
    INSERT,
    // SELECT
};

// Base struct for all statements (Polymorphism)
struct SQLStatement {
    StatementType type;
    virtual ~SQLStatement() = default;
};

// Specific struct for "CREATE TABLE"
struct CreateStatement : public SQLStatement {
    CreateStatement() { type = StatementType::CREATE_TABLE; }

    std::string tableName;
    std::vector<Table::Column> columns;
};

// Specific struct for "INSERT"
struct InsertStatement : public SQLStatement {
    InsertStatement() { type = StatementType::INSERT; }

    std::string tableName;
    std::vector<std::string> values;
};
#pragma once
#include <string>
#include <vector>
#include "Table.h" 

enum class StatementType {
    CREATE_TABLE,
    INSERT,
    SELECT,
    DELETE,
    DROP_TABLE,
    UNKNOWN
};

// Base struct for all statements (Polymorphism)
struct SQLStatement {
    StatementType type = StatementType::UNKNOWN;
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

    std::vector<std::string> columns;
    std::string tableName;

    // WHERE
    std::string filterColumn;
    std::string filterOperator;
    std::string filterValue;
};

// DELETE FROM users;
// DELETE FROM users WHERE id = 10;
struct DeleteStatement : public SQLStatement {
    DeleteStatement() { type = StatementType::DELETE; }

    std::string tableName;

    // WHERE
    std::string filterColumn;
    std::string filterOperator;
    std::string filterValue;
};

// DROP TABLE users;
// DROP TABLE IF EXISTS students;
struct DropTableStatement : public SQLStatement {
    DropTableStatement() { type = StatementType::DROP_TABLE; }

    std::string tableName;

    // IF EXISTS
    bool ifExistsCondition = false;
};

inline std::ostream& operator<<(std::ostream& os, StatementType type) {
    switch (type) {
    case StatementType::CREATE_TABLE: os << "CREATE_TABLE"; break;
    case StatementType::INSERT:       os << "INSERT";       break;
    case StatementType::SELECT:       os << "SELECT";       break;
    case StatementType::DELETE:       os << "DELETE";       break;
    case StatementType::DROP_TABLE:   os << "DROP_TABLE";   break;
    case StatementType::UNKNOWN:      os << "UNKNOWN";      break;
    default:                          os << "[Invalid Type]"; break;
    }
    return os;
}
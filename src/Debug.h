#include <iostream>
#include "Tokenizer.h"
#include "Table.h"
#include "Parser.h"
#include "SQLStatement.h"

inline void TestCreateTable(const std::unique_ptr<SQLStatement>& statement) {
    auto* createStmt = dynamic_cast<CreateStatement*>(statement.get());

    std::cout << "SUCCESS: Parser built a CREATE TABLE object.\n";
    std::cout << "Target Table: " << createStmt->tableName << "\n";
    std::cout << "Columns Found:\n";

    for (const auto& col : createStmt->columns) {
        std::cout << "  - " << col.name
            << " (Type: " << col.type << ")\n";
    }
}

inline void TestInsert(const std::unique_ptr<SQLStatement>& statement) {
    auto* insertStmt = dynamic_cast<InsertStatement*>(statement.get());

    std::cout << "SUCCESS: Parser built an INSERT object.\n";
    std::cout << "Target Table: " << insertStmt->tableName << "\n";
    std::cout << "Values Found: ";

    for (const auto& val : insertStmt->values) {
        std::cout << val << " ";
    }

    std::cout << "(All strings)" << "\n";
}

inline void TestSelect(const std::unique_ptr<SQLStatement>& statement) {
    auto* selectStmt = dynamic_cast<SelectStatement*>(statement.get());

    std::cout << "SUCCESS: Parser built a SELECT object.\n";
    std::cout << "Target Table: " << selectStmt->tableName << "\n";
    std::cout << "Columns Found: ";

    for (const auto& col : selectStmt->columns) {
        std::cout << col << " ";
    }

    if (!selectStmt->whereClause.has_where) {
        std::cout << "\n\nWHERE clause was not used\n";
    }
    else {
        std::cout << "\n\nWHERE clause was used";
        std::cout << "\nFilter column: " << selectStmt->whereClause.column;
        std::cout << "\nFilter operator: " << selectStmt->whereClause.op;
        std::cout << "\nFilter value: " << selectStmt->whereClause.value;
        std::cout << "\n";
    }
}

inline void TestUpdate(const std::unique_ptr<SQLStatement>& statement) {
    auto* updateStmt = dynamic_cast<UpdateStatement*>(statement.get());

    std::cout << "SUCCESS: Parser built an UPDATE object.\n";
    std::cout << "Target Table: " << updateStmt->tableName << "\n";
    std::cout << "Columns Found: ";

    for (const auto& col : updateStmt->columns) {
        std::cout << col.first <<" "<< col.second << ", ";
    }

    if (!updateStmt->whereClause.has_where) {
        std::cout << "\n\nWHERE clause was not used\n";
    }
    else {
        std::cout << "\n\nWHERE clause was used";
        std::cout << "\nFilter column: " << updateStmt->whereClause.column;
        std::cout << "\nFilter operator: " << updateStmt->whereClause.op;
        std::cout << "\nFilter value: " << updateStmt->whereClause.value;
        std::cout << "\n";
    }
}

inline void TestDelete(const std::unique_ptr<SQLStatement>& statement) {
    auto* deleteStmt = dynamic_cast<DeleteStatement*>(statement.get());

    std::cout << "SUCCESS: Parser built a DELETE object.\n";
    std::cout << "Target Table: " << deleteStmt->tableName << "\n";

    if (!deleteStmt->whereClause.has_where) {
        std::cout << "\nWHERE clause was not used\n";
    }
    else {
        std::cout << "\nWHERE clause was used";
        std::cout << "\nFilter column: " << deleteStmt->whereClause.column;
        std::cout << "\nFilter operator: " << deleteStmt->whereClause.op;
        std::cout << "\nFilter value: " << deleteStmt->whereClause.value;
        std::cout << "\n";
    }
}

inline void TestDropTable(const std::unique_ptr<SQLStatement>& statement) {
    auto* dropStmt = dynamic_cast<DropTableStatement*>(statement.get());

    std::cout << "SUCCESS: Parser built a DROP TABLE object.\n";
    std::cout << "Target Table: " << dropStmt->tableName << "\n";
}

inline void TestObject(const std::unique_ptr<SQLStatement>& statement) {

    if (!statement) return; // to handle null pointers

    switch (statement->type) {
    case StatementType::CREATE_TABLE:
        TestCreateTable(statement);
        break;

    case StatementType::INSERT:
        TestInsert(statement);
        break;

    case StatementType::SELECT:
        TestSelect(statement);
        break;

    case StatementType::UPDATE:
        TestUpdate(statement);
        break;

    case StatementType::DELETE:
        TestDelete(statement);
        break;

    case StatementType::DROP_TABLE:
        TestDropTable(statement);
        break;

    default:
        std::cout << "Unknown statement type encountered.\n";
        break;
    }
}


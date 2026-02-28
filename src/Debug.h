#include <iostream>
#include "Tokenizer.h"
#include "Table.h"
#include "Parser.h"

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

    if (selectStmt->filterColumn == "") {
        std::cout << "\n\nWHERE clause was not used\n";
    }
    else {
        std::cout << "\n\nWHERE clause was used";
        std::cout << "\nFilter column: " << selectStmt->filterColumn;
        std::cout << "\nFilter operator: " << selectStmt->filterOperator;
        std::cout << "\nFilter value: " << selectStmt->filterValue;
        std::cout << "\n";
    }
}

inline void TestDelete(const std::unique_ptr<SQLStatement>& statement) {
    auto* selectStmt = dynamic_cast<DeleteStatement*>(statement.get());

    std::cout << "SUCCESS: Parser built a DELETE object.\n";
    std::cout << "Target Table: " << selectStmt->tableName << "\n";

    if (selectStmt->filterColumn == "") {
        std::cout << "\n\nWHERE clause was not used\n";
    }
    else {
        std::cout << "\nWHERE clause was used";
        std::cout << "\nFilter column: " << selectStmt->filterColumn;
        std::cout << "\nFilter operator: " << selectStmt->filterOperator;
        std::cout << "\nFilter value: " << selectStmt->filterValue;
        std::cout << "\n";
    }
}

inline void TestDropTable(const std::unique_ptr<SQLStatement>& statement) {
    auto* selectStmt = dynamic_cast<DropTableStatement*>(statement.get());

    std::cout << "SUCCESS: Parser built a DROP TABLE object.\n";
    std::cout << "Target Table: " << selectStmt->tableName << "\n";
    
    // if (selectStmt->ifExistsCondition) {
    //     std::cout << "\nIF EXISTS clause was used\n";
    // }
    // else {
    //     std::cout << "\nIF EXISTS clause was not used\n";
    // }
}

inline void TestObjects(std::string command) {

    std::cout << "Command  : " << command << "\n";

    /// 1. Tokenize
    Tokenizer tokenizer(command);
    std::vector<Token> tokens = tokenizer.tokenize();

    std::cout << "Tokenized: ";
    for (auto token : tokens) {
        std::cout << token.type << " ";
    }
    std::cout << "\n" << std::string(50, '-') << "\n";

    /// 2. Parse
    Parser parser(tokens);
    std::unique_ptr<SQLStatement> statement = parser.parse();

    /// 3. Prove it worked
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


#include <iostream>
#include "Tokenizer.h"
#include "Table.h"
#include "Parser.h"

int main() {
    // std::string command = "CREATE table employees (id INTEGER, name TEXT, salary FLOAT);";
    // std::string command = "INSERT INTO employees VALUES (1, 'Mihnea', 465.69);";
    // std::string command = "SELECT id, name, salary FROM employees;";
     std::string command = "SELECT id, name, salary FROM employees WHERE salary < 14;";

    std::cout << "Command  : " << command << "\n";

    try {
        // 1. Tokenize
        Tokenizer tokenizer(command);
        std::vector<Token> tokens = tokenizer.tokenize();

        std::cout << "Tokenized: ";
        for (auto token : tokens) {
            std::cout << token.type << " ";
        }
        std::cout << "\n" << std::string(50, '-') << "\n";

        // 2. Parse
        Parser parser(tokens);
        std::unique_ptr<SQLStatement> statement = parser.parse();

        // 3. Prove it worked
        if (statement && statement->type == StatementType::CREATE_TABLE) {
            // Safely cast to our specific CreateStatement type
            auto* createStmt = dynamic_cast<CreateStatement*>(statement.get());

            std::cout << "SUCCESS: Parser built a CREATE TABLE object.\n";
            std::cout << "Target Table: " << createStmt->tableName << "\n";
            std::cout << "Columns Found:\n";

            for (const auto& col : createStmt->columns) {
                std::cout << "  - " << col.name
                    << " (Type: " << col.type << ")\n";
            }
        }
        else if (statement && statement->type == StatementType::INSERT) {
            auto* insertStmt = dynamic_cast<InsertStatement*>(statement.get());

            std::cout << "SUCCESS: Parser built an INSERT object.\n";
            std::cout << "Target Table: " << insertStmt->tableName << "\n";
            std::cout << "Values Found: ";

            for (const auto& val : insertStmt->values) {
                std::cout << val << " ";
            }

            std::cout << "(All strings)" << "\n";
        }
        else if (statement && statement->type == StatementType::SELECT) {
            auto* selectStmt = dynamic_cast<SelectStatement*>(statement.get());

            std::cout << "SUCCESS: Parser built a SELECT object.\n";
            std::cout << "Target Table: " << selectStmt->tableName << "\n";
            std::cout << "Columns Found: ";

            for (const auto& col : selectStmt->columns) {
                std::cout << col << " ";
            }
            
            if (selectStmt->filterColumn == "") { // !!!
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
    }
    catch (const std::exception& e) {
        std::cerr << "Syntax Error: " << e.what() << "\n";
    }

    return 0;
}
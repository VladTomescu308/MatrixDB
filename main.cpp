#include <iostream>
#include "Tokenizer.h"
#include "Table.h"
#include "Parser.h"

int main() {
    std::string command = "CREATE TABLE employees (id INTEGER, name TEXT, salary FLOAT);";

    std::cout << "Executing: " << command << "\n";
    std::cout << std::string(50, '-') << "\n";

    try {
        // 1. Tokenize
        Tokenizer tokenizer(command);
        std::vector<Token> tokens = tokenizer.tokenize();

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
                    << " (Type ID: " << (int)col.type << ")\n";
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Syntax Error: " << e.what() << "\n";
    }

    return 0;
}
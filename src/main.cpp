#include <iostream>
#include <unordered_map>
#include "Tokenizer.h"
#include "Table.h"
#include "Parser.h"
#include "Debug.h"

#define ANSI_COLOR_RESET    "\033[0m"
#define ANSI_COLOR_RED      "\033[31m"  
#define ANSI_COLOR_GREEN    "\033[32m"
#define ANSI_COLOR_BLUE     "\033[34m"

int main() {

    // DEBUG COMMANDS
    //  CREATE table employees (id INTEGER, name TEXT, salary FLOAT);
    //  INSERT INTO employees VALUES (1, 'Mihnea', 465.69);
    //  SELECT id, name, salary FROM employees;
    //  SELECT id, name, salary FROM employees WHERE salary < 14;
    //  DELETE FROM employees;
    //  DELETE FROM employees WHERE id = 1;
    //  DROP table employees;

    std::cout << "======================================\n";
    std::cout << " Welcome to MatrixDB (v1.0)\n";
    std::cout << " Type 'EXIT' to quit.\n";
    std::cout << "======================================\n";

    std::string input;
    std::unordered_map<std::string, Table> database;

    while (true) {
        std::cout << "\n" << ANSI_COLOR_BLUE << "MatrixDB> " << ANSI_COLOR_RESET;
        std::getline(std::cin, input);

        if (input.empty()) {
            continue;
        }

        if (input == "EXIT;" || input == "exit;" || input == "EXIT" || input == "exit") {
            std::cout << "Shutting down MatrixDB. Goodbye!\n";
            break;
        }

        try {
            Tokenizer tokenizer(input);
            std::vector<Token> tokens = tokenizer.tokenize();

            Parser parser(tokens);
            std::unique_ptr<SQLStatement> statement = parser.parse();

            TestObject(statement);

            // std::cout << ">> [Success] Parsed command of type: "
            //     << statement->type << "\n";

            
            if (statement->type == StatementType::CREATE_TABLE) {

                auto* createStmt = dynamic_cast<CreateStatement*>(statement.get());

                if (database.find(createStmt->tableName) != database.end()) {
                    std::cerr << ANSI_COLOR_RED << "Error: " << ANSI_COLOR_RESET << "Table '"<< createStmt->tableName << "' already exists.\n";
                    continue;
                }

                Table newTable(createStmt->tableName);

                for (const auto& col : createStmt->columns) {
                    newTable.add_column(col.name, col.type);
                }

                database.insert({ createStmt->tableName, newTable });

                std::cout << ">> Success: Table '" << createStmt->tableName << "' created with "
                    << createStmt->columns.size() << " columns.\n";
            }

            else if (statement->type == StatementType::INSERT) {

                auto* insertStmt = dynamic_cast<InsertStatement*>(statement.get());
                auto it = database.find(insertStmt->tableName);
                
                if (it != database.end()) {
                    Table& table = it->second;

                    if (table.insert_row(insertStmt->values)) {
                        std::cout << ">> Success: Inserted the values: ";
                        for (int i = 0; i < insertStmt->values.size() - 1; i++) {
                            std::cout << insertStmt->values[i] << ", ";
                        }
                        std::cout << insertStmt->values.back();
                        std::cout << " into " << insertStmt->tableName << ".\n";
                    }
                }
                else {
                    std::cerr << ANSI_COLOR_RED << "Error: " << ANSI_COLOR_RESET << "Table '" << insertStmt->tableName << "' doesn't exist.\n";
                    continue;
                }
            }

            else if (statement->type == StatementType::SELECT) {

                auto* selectStmt = dynamic_cast<SelectStatement*>(statement.get());
                auto it = database.find(selectStmt->tableName);

                if (selectStmt->columns[0] == "*") {
                    Table& table = it->second;
                    table.print_table();
                }

                else if (it != database.end()) {
                    Table& table = it->second;
                    table.print_table(selectStmt->columns);
                }
                
                else {
                    std::cerr << ANSI_COLOR_RED << "Error: " << ANSI_COLOR_RESET << "Table '" << selectStmt->tableName << "' doesn't exist.\n";
                    continue;
                }

            }
        }
        catch (const std::exception& e) {
            std::cerr << ANSI_COLOR_RED << "Error: " << ANSI_COLOR_RESET << e.what() << "\n";
        }
    }

    return 0;
}
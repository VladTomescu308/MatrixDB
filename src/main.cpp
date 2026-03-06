#include <iostream>
#include "Tokenizer.h"
#include "Table.h"
#include "Parser.h"
#include "Debug.h"

int main() {

    // DEBUG COMMANDS
    //  CREATE table employees (id INTEGER, name TEXT, salary FLOAT);
    //  INSERT INTO employees VALUES (1, 'Mihnea', 465.69);
    //  SELECT id, name, salary FROM employees;
    //  SELECT id, name, salary FROM employees WHERE salary < 14;
    //  DELETE FROM employees;
    //  CREATE table employees (id INTEGER, name TEXT, salary FLOAT);
    //  DROP table employees;

    std::cout << "======================================\n";
    std::cout << " Welcome to MatrixDB (v1.0)\n";
    std::cout << " Type 'EXIT' to quit.\n";
    std::cout << "======================================\n\n";

    std::string input;

    while (true) {
        std::cout << "MatrixDB> ";
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

            // std::cout << ">> [Success] Parsed command of type: "
            //     << statement->type << "\n";

            TestObject(statement);

            // if (statement->type == StatementType::CREATE) { ... }
            // if (statement->type == StatementType::INSERT) { ... }
        }
        catch (const std::exception& e) {
            std::cerr << "\033[31mError:\033[0m " << e.what() << "\n";
        }
    }

    return 0;
}
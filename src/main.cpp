#include <iostream>
#include "Tokenizer.h"
#include "Table.h"
#include "Parser.h"
#include "Debug.h"

int main() {

    // DEBUG COMMANDS
    // std::string command = "CREATE table employees (id INTEGER, name TEXT, salary FLOAT);";
    // std::string command = "INSERT INTO employees VALUES (1, 'Mihnea', 465.69);";
    std::string command = "SELECT id, name, salary FROM employees;";
    // std::string command = "SELECT id, name, salary FROM employees WHERE salary < 14;";
    // std::string command = "DELETE FROM employees WHERE id = 5;";
    // std::string command = "DROP table employees;";

    try {
        TestObjects(command);
    }
    catch (const std::exception& e) {
        std::cerr << "Syntax Error: " << e.what() << "\n";
    }

    return 0;
}
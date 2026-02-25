#include "Table.h"

Table::Table(const std::string& name)
    : m_name(name) {
}

void Table::add_column(const std::string& name, DataType type) {
    m_columns.push_back({ name, type });
}

bool Table::insert_row(const std::vector<std::string>& row_data) {
    if (row_data.size() != m_columns.size()) {
        std::cerr << "Error: Table '" << m_name << "' expects "
            << m_columns.size() << " values, but got "
            << row_data.size() << ".\n";
        return false;
    }

    // Later, we can add logic here to check if an INTEGER column 
    // actually contains a number, but for now, we just save it!
    m_rows.push_back(row_data);
    return true;
}

void Table::print_table() const {
    std::cout << "\nTABLE: " << m_name << "\n";
    std::cout << std::string(50, '=') << "\n";

    // 1. Print Header
    for (const auto& col : m_columns) {
        std::cout << std::left << std::setw(15) << col.name;
    }
    std::cout << "\n" << std::string(50, '-') << "\n";

    // 2. Print Data (The Matrix)
    for (const auto& row : m_rows) {
        for (const auto& cell : row) {
            std::cout << std::left << std::setw(15) << cell;
        }
        std::cout << "\n";
    }
    std::cout << std::string(50, '=') << "\n\n";
}

std::string Table::get_name() const {
    return m_name;
}

const std::vector<Table::Column>& Table::get_columns() const {
    return m_columns;
}

const std::vector<std::vector<std::string>>& Table::get_rows() const {
    return m_rows;
}
#include "Table.h"

Table::Table(const std::string& name)
    : m_name(name) {
}

void Table::add_column(const std::string& name, DataType type) {
    m_columns.push_back({ name, type });
}

bool Table::insert_row(const std::vector<std::string>& row_data) {

    if (row_data.size() != m_columns.size()) {
        std::cerr << "\033[31mError:\033[0m Table '" << m_name << "' expects "
            << m_columns.size() << " values, but got " << row_data.size() << ".\n";
        return false;
    }

    for (int i = 0; i < m_columns.size(); i++) {
        try {
            if (m_columns[i].type == DataType::INTEGER) {
                (void)std::stoi(row_data[i]);
            }
            else if (m_columns[i].type == DataType::FLOAT) {
                (void)std::stof(row_data[i]);
            }
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "\033[31mValidation Error:\033[0m Column '" << m_columns[i].name
                << "' expects a number, but got '" << row_data[i] << "'.\n";
            return false;
        }
        catch (const std::out_of_range& e) {
            std::cerr << "\033[31mValidation Error:\033[0m Value '" << row_data[i]
                << "' is out of range for column '" << m_columns[i].name << "'.\n";
            return false;
        }
    }

    m_rows.push_back(row_data);
    return true;
}

void Table::print_table() const {
    std::cout << "\nTABLE: " << m_name << "\n";
    std::cout << std::string(50, '=') << "\n";

    // Print Header
    for (const auto& col : m_columns) {
        std::cout << std::left << std::setw(15) << col.name;
    }
    std::cout << "\n" << std::string(50, '-') << "\n";

    // Print Data
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

std::ostream& operator<<(std::ostream& os, DataType type) {
    switch (type) {
    case DataType::INTEGER: return os << "INTEGER";
    case DataType::TEXT:    return os << "TEXT";
    case DataType::FLOAT:   return os << "FLOAT";
    default:                return os << "UNKNOWN";
    }
}
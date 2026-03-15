#include "Table.h"
#include "SQLStatement.h"

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

bool Table::evaluate_where(const std::vector<std::string>& row, size_t col_index, const std::string& op, const std::string& target_value, DataType type) const {

    std::string cell_value = row[col_index];

    try {
        if (type == DataType::INTEGER) {
            int cell_int = std::stoi(cell_value);
            int target_int = std::stoi(target_value);

            if (op == "=") return cell_int == target_int;
            if (op == "<") return cell_int < target_int;
            if (op == ">") return cell_int > target_int;
            if (op == "<=") return cell_int <= target_int;
            if (op == ">=") return cell_int >= target_int;
        }

        else if (type == DataType::FLOAT) {
            float cell_float = std::stof(cell_value);
            float target_float = std::stof(target_value);

            if (op == "=") return cell_float == target_float;
            if (op == "<") return cell_float < target_float;
            if (op == ">") return cell_float > target_float;
            if (op == "<=") return cell_float <= target_float;
            if (op == ">=") return cell_float >= target_float;
        }

        else if (type == DataType::TEXT) {
            if (op == "=") return cell_value == target_value;
            if (op == "<") return cell_value < target_value;
            if (op == ">") return cell_value > target_value;
        }
    }
    catch (const std::exception& e) {
        return false;
    }

    return false;
}

void Table::print_table(const WhereClause& whereClause) const {

    const int cell_width = 18;

    std::string separator = "";
    for (size_t i = 0; i < m_columns.size(); ++i) {
        separator += "+" + std::string(cell_width + 2, '-');
    }
    separator += "+";

    std::cout << "\nTABLE: " << m_name << "\n";

    std::cout << separator << "\n";

    for (const auto& col : m_columns) {
        std::cout << "| " << std::left << std::setw(cell_width) << col.name << " ";
    }
    std::cout << "|\n";

    std::cout << separator << "\n";

    if (!whereClause.has_where) {
        for (const auto& row : m_rows) {
            for (const auto& cell : row) {
                std::cout << "| " << std::left << std::setw(cell_width) << cell << " ";
            }
            std::cout << "|\n";
        }
    }
    else {
        int where_col_index = -1;
        DataType where_col_type;
        bool found = false;

        for (size_t i = 0; i < m_columns.size(); ++i) {
            if (m_columns[i].name == whereClause.column) {
                where_col_index = i;
                where_col_type = m_columns[i].type;
                found = true;
                break;
            }
        }

        if (!found) {
            std::cerr << "\033[31mError:\033[0m WHERE column '" << whereClause.column << "' does not exist.\n";
            return;
        }

        for (const auto& row : m_rows) {
            // Doesn't pass the test
            if (!evaluate_where(row, where_col_index, whereClause.op, whereClause.value, where_col_type)) {
                continue;
            }

            // Passes the test
            for (const auto& cell : row) {
                std::cout << "| " << std::left << std::setw(cell_width) << cell << " ";
            }
            std::cout << "|\n";
        }
    }
    std::cout << separator << "\n\n";   
}

void Table::print_table(const WhereClause& whereClause, std::vector<std::string>& columns) const {
    
    std::vector<int> col_indicies;

    for (const auto& col : columns) {
        bool found = false;
        for (size_t i = 0; i < m_columns.size(); ++i) {
            if (m_columns[i].name == col) {
                col_indicies.push_back(i);
                found = true;
                break;
            }
        }
        if (!found) {
            std::cerr << "\033[31mError:\033[0m column '" << col << "' does not exist.\n";
            return;
        }
    }

    const int cell_width = 18;

    std::string separator = "";
    for (size_t i = 0; i < columns.size(); ++i) {
        separator += "+" + std::string(cell_width + 2, '-');
    }
    separator += "+";

    std::cout << "\nTABLE: " << m_name << "\n";

    std::cout << separator << "\n";

    
    for (const auto& col : columns) {
        std::cout << "| " << std::left << std::setw(cell_width) << col << " ";
    }
    std::cout << "|\n";

    std::cout << separator << "\n";

    if (!whereClause.has_where) {
        for (const auto& row : m_rows) {
            for (size_t i : col_indicies) {
                std::cout << "| " << std::left << std::setw(cell_width) << row[i] << " ";
            }
            std::cout << "|\n";
        }
    }
    else {
        int where_col_index = -1;
        DataType where_col_type;
        bool found = false;

        for (size_t i = 0; i < m_columns.size(); ++i) {
            if (m_columns[i].name == whereClause.column) {
                where_col_index = i;
                where_col_type = m_columns[i].type;
                found = true;
                break;
            }
        }

        if (!found) {
            std::cerr << "\033[31mError:\033[0m WHERE column '" << whereClause.column << "' does not exist.\n";
            return;
        }

        for (const auto& row : m_rows) {
            // Doesn't pass the test
            if (!evaluate_where(row, where_col_index, whereClause.op, whereClause.value, where_col_type)) {
                continue;
            }
            // Passes the test
            for (size_t i : col_indicies) {
                std::cout << "| " << std::left << std::setw(cell_width) << row[i] << " ";
            }
            std::cout << "|\n";
        }
    }
    std::cout << separator << "\n\n";
}

int Table::delete_rows(const WhereClause& whereClause) {

    if (!whereClause.has_where) {
        int rows_deleted = m_rows.size();
        m_rows.clear();
        return rows_deleted;
    }

    int where_col_index = -1;
    DataType where_col_type;
    bool found = false;

    for (size_t i = 0; i < m_columns.size(); ++i) {
        if (m_columns[i].name == whereClause.column) {
            where_col_index = i;
            where_col_type = m_columns[i].type;
            found = true;
            break;
        }
    }

    if (!found) {
        std::cerr << "\033[31mError:\033[0m WHERE column '" << whereClause.column << "' does not exist.\n";
        return 0;
    }

    int deleted_count = 0;
    auto it = m_rows.begin();

    while (it != m_rows.end()) {
        
        if (evaluate_where(*it, where_col_index, whereClause.op, whereClause.value, where_col_type)) {
            it = m_rows.erase(it);
            deleted_count++;
        }
        else {
            ++it;
        }
    }

    return deleted_count;
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
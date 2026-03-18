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
        catch (const std::invalid_argument) {
            std::cerr << "\033[31mValidation Error:\033[0m Column '" << m_columns[i].name
                << "' expects a number, but got '" << row_data[i] << "'.\n";
            return false;
        }
        catch (const std::out_of_range) {
            std::cerr << "\033[31mValidation Error:\033[0m Value '" << row_data[i]
                << "' is out of range for column '" << m_columns[i].name << "'.\n";
            return false;
        }
    }

    m_rows.push_back(row_data);
    return true;
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
        size_t where_col_index = -1;
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

void Table::print_table(const WhereClause& whereClause, const std::vector<std::string>& columns) const {
    
    std::vector<size_t> col_indicies;

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
        size_t where_col_index = -1;
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

size_t Table::update_rows(const std::vector<std::pair<std::string, std::string>>& targetColumns, const WhereClause& whereClause) {
    size_t updatedRows = 0;

    size_t where_col_idx = -1;
    DataType where_col_type;

    if (whereClause.has_where) {
        for (size_t i = 0; i < m_columns.size(); ++i) {
            if (m_columns[i].name == whereClause.column) {
                where_col_idx = i;
                where_col_type = m_columns[i].type;
                break;
            }
        }
        if (where_col_idx == -1) {
            throw std::runtime_error("\033[31mError:\033[0m WHERE column '" + whereClause.column + "' does not exist.\n");
            return 0;
        }
    }

    std::vector<std::pair<size_t, std::string>> valid_updates;

    for (const auto& col : targetColumns) {
        bool found = false;
        for (size_t i = 0; i < m_columns.size(); ++i) {
            if (col.first == m_columns[i].name) {
                try {
                    if (m_columns[i].type == DataType::INTEGER) {
                        (void)std::stoi(col.second);
                    }
                    else if (m_columns[i].type == DataType::FLOAT) {
                        (void)std::stof(col.second);
                    }
                }
                catch (const std::exception) {
                    throw std::runtime_error("Type mismatch: Cannot assign value '" + col.second + "' to column '" + col.first + "'.");
                }

                valid_updates.emplace_back(i, col.second);
                found = true;
                break;
            }
        }
        if (!found) {
            throw std::runtime_error("\033[31mError:\033[0m SET column '" + col.first + "' does not exist.\n");
        }
    }

    if (valid_updates.empty()) {
        return 0;
    }

    for (size_t r = 0; r < m_rows.size(); ++r) {

        if (whereClause.has_where) {
            if (!evaluate_where(m_rows[r], where_col_idx, whereClause.op, whereClause.value, where_col_type)) {
                continue;
            }
        }

        bool row_changed = false;
        for (const auto& update : valid_updates) {
            size_t col_index = update.first;
            const std::string& new_value = update.second;

            if (m_rows[r][col_index] != new_value) {
                m_rows[r][col_index] = new_value;
                row_changed = true;
            }
        }

        if (row_changed) {
            updatedRows++;
        }
    }
    return updatedRows;
}

size_t Table::delete_rows(const WhereClause& whereClause) {

    if (!whereClause.has_where) {
        size_t rows_deleted = m_rows.size();
        m_rows.clear();
        return rows_deleted;
    }

    size_t where_col_index = -1;
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
    catch (const std::exception) {
        return false;
    }

    return false;
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
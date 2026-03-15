#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

// Forward declaration to avoid circular include with SQLStatement.h
struct WhereClause;

enum class DataType {
    INTEGER,
    TEXT,
    FLOAT
};

class Table {
public:
    struct Column {
        std::string name;
        DataType type;
    };

    explicit Table(const std::string& name);

    void add_column(const std::string& name, DataType type);
    bool insert_row(const std::vector<std::string>& row_data);
    
    bool evaluate_where(const std::vector<std::string>& row, size_t col_index, const std::string& op, const std::string& target_value, DataType type) const;

    void print_table(const WhereClause& whereClause) const;
    void print_table(const WhereClause& whereClause, std::vector<std::string>& columns) const;

    int delete_rows(const WhereClause& whereClause);

    // Getters
    std::string get_name() const;
    const std::vector<Column>& get_columns() const;
    const std::vector<std::vector<std::string>>& get_rows() const;

private:
    std::string m_name;
    std::vector<Column> m_columns;

    // The Matrix: A vector of rows, where each row is a vector of strings
    std::vector<std::vector<std::string>> m_rows;
};

std::ostream& operator<<(std::ostream& os, DataType type);
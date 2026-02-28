#pragma once
#include <vector>
#include <memory>
#include <stdexcept>
#include "Tokenizer.h"
#include "SQLStatement.h"

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    std::unique_ptr<SQLStatement> parse();

private:
    const std::vector<Token>& m_tokens;
    size_t m_pos;

    // Helpers to navigate the tokens safely
    const Token& current() const;
    void advance();
    bool match(TokenType type) const;
    Token consume(TokenType type, const std::string& error_message);

    // Parsing specific commands
    std::unique_ptr<CreateStatement> parse_create_table();
    std::unique_ptr<InsertStatement> parse_insert();
    std::unique_ptr<SelectStatement> parse_select();
    std::unique_ptr<DeleteStatement> parse_delete();
    std::unique_ptr<DropTableStatement> parse_drop_table();
    DataType parse_data_type();
};
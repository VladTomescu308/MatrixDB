#include "Parser.h"

Parser::Parser(const std::vector<Token>& tokens)
    : m_tokens(tokens), m_pos(0) {
}

std::unique_ptr<SQLStatement> Parser::parse() {
    if (m_tokens.empty() || current().type == TokenType::END_OF_FILE) {
        return nullptr;
    }

    if (match(TokenType::CREATE)) {
        return parse_create_table();
    }

	if (match(TokenType::INSERT)) {
		advance();
		if (match(TokenType::INTO)) {
            advance();
            // return parse_insert();
        }
    }

	if (match(TokenType::SELECT)) {
        advance();
        // return parse_select();
    }

    throw std::runtime_error("Unsupported or unrecognized command.");
}

std::unique_ptr<CreateStatement> Parser::parse_create_table() {
    advance();

    consume(TokenType::TABLE, "Expected 'TABLE' after 'CREATE'");

    auto stmt = std::make_unique<CreateStatement>();

    // 1. Get the table name
    Token nameToken = consume(TokenType::IDENTIFIER, "Expected table name");
    stmt->tableName = nameToken.value;

    // 2. Open parentheses
    consume(TokenType::LPAREN, "Expected '(' after table name");

    // 3. Loop through columns
    bool parsingColumns = true;
    while (parsingColumns) {
        Token colName = consume(TokenType::IDENTIFIER, "Expected column name");
        DataType colType = parse_data_type();

        stmt->columns.push_back({ colName.value, colType });

        // Check what comes next: a comma (more columns) or a closing parenthesis
        if (match(TokenType::COMMA)) {
            advance(); // Skip comma, loop continues
        }
        else if (match(TokenType::RPAREN)) {
            advance(); // Skip ')', loop ends
            parsingColumns = false;
        }
        else {
            throw std::runtime_error("Expected ',' or ')' in column definition");
        }
    }

    // 4. Require a semicolon at the end
    consume(TokenType::SEMICOLON, "Expected ';' at the end of statement");

    return stmt;
}

DataType Parser::parse_data_type() {
    if (match(TokenType::INTEGER)) { advance(); return DataType::INTEGER; }
    if (match(TokenType::TEXT)) { advance(); return DataType::TEXT; }
    if (match(TokenType::FLOAT)) { advance(); return DataType::FLOAT; }

    throw std::runtime_error("Expected column data type (INTEGER, TEXT, FLOAT)");
}



// --- Navigation Helpers ---

const Token& Parser::current() const {
    if (m_pos >= m_tokens.size()) return m_tokens.back(); // Return EOF if out of bounds
    return m_tokens[m_pos];
}

void Parser::advance() {
    if (m_pos < m_tokens.size()) m_pos++;
}

bool Parser::match(TokenType type) const {
    return current().type == type;
}

Token Parser::consume(TokenType type, const std::string& error_message) {
    if (match(type)) {
        Token token = current();
        advance();
        return token;
    }
    throw std::runtime_error(error_message + " (Found: '" + current().value + "')");
}
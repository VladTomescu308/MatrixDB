#include "Parser.h"

Parser::Parser(const std::vector<Token>& tokens)
    : m_tokens(tokens), m_pos(0) {
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

// --- Parsing helpers ---

DataType Parser::parse_data_type() {
    if (match(TokenType::INTEGER)) { advance(); return DataType::INTEGER; }
    if (match(TokenType::TEXT)) { advance(); return DataType::TEXT; }
    if (match(TokenType::FLOAT)) { advance(); return DataType::FLOAT; }

    throw std::runtime_error("Expected column data type (INTEGER, TEXT, FLOAT)");
}

WhereClause Parser::parse_where() {
    WhereClause wc;

    if (!match(TokenType::WHERE)) {
        return wc;
    }

    advance();
    wc.has_where = true;

    wc.column = consume(TokenType::IDENTIFIER, "Expected a column name for the condition").value;

    switch (current().type) {
    case TokenType::EQUALS:
    case TokenType::LOWER:
    case TokenType::GREATER:
        wc.op = current().value;
        advance();
        break;
    default:
        throw std::runtime_error("Expected a comparison operator (=, <, >)");
    }

    switch (current().type) {
    case TokenType::INT_LITERAL:
    case TokenType::FLOAT_LITERAL:
    case TokenType::STRING_LITERAL:
        wc.value = current().value;
        advance();
        break;
    default:
        throw std::runtime_error("Expected a filter value (int, string, float)");
    }

    return wc;
}

std::unique_ptr<SQLStatement> Parser::parse() {
    if (m_tokens.empty() || current().type == TokenType::END_OF_FILE) {
        return nullptr;
    }

    if (match(TokenType::CREATE)) {
        advance();
        consume(TokenType::TABLE, "Expected 'TABLE' after 'CREATE'");
        return parse_create_table();
    }

    if (match(TokenType::INSERT)) {
        advance();
        consume(TokenType::INTO, "Expected 'INTO' after 'INSERT'");
        return parse_insert();
    }

    if (match(TokenType::SELECT)) {
        advance();
        return parse_select();
    }

    if (match(TokenType::DELETE)) {
        advance();
        consume(TokenType::FROM, "Expected 'FROM' after 'Delete'");
        return parse_delete();
    }

    if (match(TokenType::DROP)) {
        advance();
        consume(TokenType::TABLE, "Expected 'TABLE' after 'DROP'");
        return parse_drop_table();
    }

    throw std::runtime_error("Unsupported or unrecognized command.");
}

// CREATE TABLE users (id INTEGER, name TEXT, balance FLOAT);
std::unique_ptr<CreateStatement> Parser::parse_create_table() {

    auto stmt = std::make_unique<CreateStatement>();

    // 1. Get the table name
    Token tableToken = consume(TokenType::IDENTIFIER, "Expected table name");
    stmt->tableName = tableToken.value;

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

// INSERT INTO users VALUES (1, Alice, 4500.50);
std::unique_ptr<InsertStatement> Parser::parse_insert() {

    auto stmt = std::make_unique<InsertStatement>();

    Token tableToken = consume(TokenType::IDENTIFIER, "Expected table name");
    stmt->tableName = tableToken.value;

    consume(TokenType::VALUES, "Expected 'VALUES'");

    consume(TokenType::LPAREN, "Expected '(' after 'VALUES'");

    bool parsingRows = true;
    while (parsingRows) {

        Token valueToken;

        if (match(TokenType::STRING_LITERAL)) {
            valueToken = consume(TokenType::STRING_LITERAL, "Expected a string value ex:'Alice'");
        }
        else if (match(TokenType::INT_LITERAL)) {
            valueToken = consume(TokenType::INT_LITERAL, "Expected an int value");
        }
        else if (match(TokenType::FLOAT_LITERAL)) {
            valueToken = consume(TokenType::FLOAT_LITERAL, "Expected a float value");
        }
        else {
            throw std::runtime_error("Expected a valid value (String, Int, or Float).");
        }

        stmt->values.push_back(valueToken.value);

        if (match(TokenType::COMMA)) {
            advance();
        }
        else if (match(TokenType::RPAREN))
        {
            parsingRows = false;
            advance();
        }
        else {
            throw std::runtime_error("Expected ',' or ')' in column definition");
        }
    }

    consume(TokenType::SEMICOLON, "Expected ';' at the end of statement");
    return stmt;
}

// SELECT * FROM users;
// SELECT id, name FROM users WHERE balance > 1000;
std::unique_ptr<SelectStatement> Parser::parse_select() {

    auto stmt = std::make_unique<SelectStatement>();
    bool parsingColumns = true;
    bool foundStar = false;
    bool expectComma = false;

    while (parsingColumns) {

        if (match(TokenType::IDENTIFIER) && !foundStar) {
            if (expectComma) {
                 throw std::runtime_error("Expected ',' between column names");
            }
            auto column = consume(TokenType::IDENTIFIER, "Expected a column name");
            stmt->columns.push_back(column.value);
            expectComma = true;
        }

        else if (match(TokenType::COMMA) && !foundStar ) {
            if (!expectComma) {
                throw std::runtime_error("Unexpected ',' in column list");
            }
            expectComma = false;
            advance();
        }

        else if (match(TokenType::STAR)) {
            if (expectComma || !stmt->columns.empty()) {
                throw std::runtime_error("Cannot mix '*' with specific columns or commas");
            }

            stmt->columns.push_back("*");
            foundStar = true;
            expectComma = true;
            advance();
        }

        else if (match(TokenType::FROM)) {
            if (!expectComma && !stmt->columns.empty()) {
                throw std::runtime_error("Unexpected 'FROM' after comma");
            }
            parsingColumns = false;
            advance();
            break;
        }

        else throw std::runtime_error("Expected 'FROM' after column names");
    }

    if (stmt->columns.empty()) {
        throw std::runtime_error("Expected at least one column or '*' after SELECT");
    }

    auto table = consume(TokenType::IDENTIFIER, "Expected a table name");
    stmt->tableName = table.value;

    stmt->whereClause = parse_where();

    consume(TokenType::SEMICOLON, "Expected ';' at the end of statement");
    return stmt;
}

// DELETE FROM users;
// DELETE FROM users WHERE id = 10;
std::unique_ptr<DeleteStatement> Parser::parse_delete() {

    auto stmt = std::make_unique<DeleteStatement>();

    auto tableName = consume(TokenType::IDENTIFIER, "Expected a table name");
    stmt->tableName = tableName.value;

    stmt->whereClause = parse_where();

    consume(TokenType::SEMICOLON, "Expected ';' at the end of statement");
    return stmt;
}

// DROP TABLE users;
std::unique_ptr<DropTableStatement> Parser::parse_drop_table() {

    auto stmt = std::make_unique<DropTableStatement>();

    auto tableName = consume(TokenType::IDENTIFIER, "Expected table name");
    stmt->tableName = tableName.value;

    consume(TokenType::SEMICOLON, "Expected ';' at the end of statement");
    return stmt;
}


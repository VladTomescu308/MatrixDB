#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

enum class TokenType {
    // Keywords
    SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, TABLE, INTO, VALUES, FROM, WHERE,

    // Data Types
    INTEGER, TEXT, FLOAT,

    // Symbols & Operators
    LPAREN, RPAREN, COMMA, STAR, EQUALS, SEMICOLON,

    // Identifiers & Literals
    IDENTIFIER,     // e.g., table names, column names
    STRING_LITERAL, // e.g., 'Alice'
    INT_LITERAL,    // e.g., 123
    FLOAT_LITERAL,  // e.g., 12.5

    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int position;
};

class Tokenizer {
public:
    Tokenizer(const std::string& input);
    std::vector<Token> tokenize();

private:
    const std::string& m_input;
    size_t m_pos;

    static const std::unordered_map<std::string, TokenType> keywords;

    // Helpers to navigate the string
    char current() const;
    char peek() const;
    void advance();
    void skip_whitespace();

    // Helpers to extract specific token types
    Token consume_identifier_or_keyword();
    Token consume_number();
    Token consume_string();
};

std::ostream& operator<<(std::ostream& os, TokenType type);
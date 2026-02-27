#include "Tokenizer.h"
#include <cctype> // Required for isspace, isdigit, isalpha

const std::unordered_map<std::string, TokenType> Tokenizer::keywords = {
    {"SELECT", TokenType::SELECT},
    {"INSERT", TokenType::INSERT},
    {"UPDATE", TokenType::UPDATE},
    {"DELETE", TokenType::DELETE},
    {"CREATE", TokenType::CREATE},
    {"DROP",   TokenType::DROP},
    {"TABLE",  TokenType::TABLE},
    {"INTO",   TokenType::INTO},
    {"VALUES", TokenType::VALUES},
    {"FROM",   TokenType::FROM},
    {"WHERE",  TokenType::WHERE},
    {"INTEGER", TokenType::INTEGER},
    {"TEXT",    TokenType::TEXT},
    {"FLOAT",   TokenType::FLOAT}
};

// Constructor
Tokenizer::Tokenizer(const std::string& input)
    : m_input(input), m_pos(0) {
}

// The Main Loop
std::vector<Token> Tokenizer::tokenize() {
    std::vector<Token> tokens;

    while (current() != '\0') {
        skip_whitespace();

        char c = current();
        if (c == '\0') break;

        // 1. Letters -> Keywords or Identifiers
        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            tokens.push_back(consume_identifier_or_keyword());
        }
        // 2. Digits -> Integers or Floats
        else if (std::isdigit(static_cast<unsigned char>(c))) {
            tokens.push_back(consume_number());
        }
        // 3. Quotes -> String Literals
        else if (c == '\'') {
            tokens.push_back(consume_string());
        }
        // 4. Symbols
        else {
            switch (c) {
            case '*': tokens.push_back({ TokenType::STAR,       "*", static_cast<int>(m_pos) }); break;
            case ',': tokens.push_back({ TokenType::COMMA,      ",", static_cast<int>(m_pos) }); break;
            case '(': tokens.push_back({ TokenType::LPAREN,     "(", static_cast<int>(m_pos) }); break;
            case ')': tokens.push_back({ TokenType::RPAREN,     ")", static_cast<int>(m_pos) }); break;
            case '=': tokens.push_back({ TokenType::EQUALS,     "=", static_cast<int>(m_pos) }); break;
            case '<': tokens.push_back({ TokenType::LOWER,      "<", static_cast<int>(m_pos) }); break;
            case '>': tokens.push_back({ TokenType::GREATER,    ">", static_cast<int>(m_pos) }); break;
            case ';': tokens.push_back({ TokenType::SEMICOLON,  ";", static_cast<int>(m_pos) }); break;
            default:  tokens.push_back({ TokenType::UNKNOWN, std::string(1, c), static_cast<int>(m_pos) }); break;
            }
            advance();
        }
    }

    tokens.push_back({ TokenType::END_OF_FILE, "", static_cast<int>(m_pos) });
    return tokens;
}

// Navigation Helpers
char Tokenizer::current() const {
    if (m_pos >= m_input.length()) return '\0';
    return m_input[m_pos];
}

char Tokenizer::peek() const {
    if (m_pos + 1 >= m_input.length()) return '\0';
    return m_input[m_pos + 1];
}

void Tokenizer::advance() {
    if (m_pos < m_input.length()) {
        m_pos++;
    }
}

void Tokenizer::skip_whitespace() {
    while (current() != '\0' && std::isspace(static_cast<unsigned char>(current()))) {
        advance();
    }
}

// Logic Helpers
Token Tokenizer::consume_identifier_or_keyword() {
    size_t start = m_pos;

    while (std::isalnum(static_cast<unsigned char>(current())) || current() == '_') {
        advance();
    }

    std::string value = m_input.substr(start, m_pos - start);

    // Create an uppercase version for checking the map safely
    std::string upper_value = value;
    for (char& c : upper_value) {
        c = std::toupper(static_cast<unsigned char>(c));
    }

    auto it = keywords.find(upper_value);
    TokenType type = (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;

    return { type, value, static_cast<int>(start) };
}

Token Tokenizer::consume_number() {
    size_t start = m_pos;
    bool is_float = false;

    while (std::isdigit(static_cast<unsigned char>(current()))) {
        advance();
    }

    if (current() == '.') {
        is_float = true;
        advance(); // consume the dot
        while (std::isdigit(static_cast<unsigned char>(current()))) {
            advance();
        }
    }

    std::string value = m_input.substr(start, m_pos - start);
    TokenType type = is_float ? TokenType::FLOAT_LITERAL : TokenType::INT_LITERAL;

    return { type, value, static_cast<int>(start) };
}

Token Tokenizer::consume_string() {
    advance(); // Skip opening quote
    size_t start = m_pos;

    while (current() != '\'' && current() != '\0') {
        advance();
    }

    std::string value = m_input.substr(start, m_pos - start);

    if (current() == '\'') {
        advance(); // Skip closing quote
    }

    return { TokenType::STRING_LITERAL, value, static_cast<int>(start - 1) };
}

// Printer formatting
std::ostream& operator<<(std::ostream& os, TokenType type) {
    switch (type) {
    case TokenType::SELECT: return os << "SELECT";
    case TokenType::INSERT: return os << "INSERT";
    case TokenType::UPDATE: return os << "UPDATE";
    case TokenType::DELETE: return os << "DELETE";
    case TokenType::CREATE: return os << "CREATE";
    case TokenType::DROP:   return os << "DROP";
    case TokenType::TABLE:  return os << "TABLE";
    case TokenType::INTO:   return os << "INTO";
    case TokenType::VALUES: return os << "VALUES";
    case TokenType::FROM:   return os << "FROM";
    case TokenType::WHERE:  return os << "WHERE";
    case TokenType::INTEGER: return os << "INTEGER";
    case TokenType::TEXT:    return os << "TEXT";
    case TokenType::FLOAT:   return os << "FLOAT";
    case TokenType::LPAREN:  return os << "LPAREN";
    case TokenType::RPAREN:  return os << "RPAREN";
    case TokenType::COMMA:   return os << "COMMA";
    case TokenType::STAR:    return os << "STAR";
    case TokenType::EQUALS:  return os << "EQUALS";
    case TokenType::LOWER:   return os << "LOWER";
    case TokenType::GREATER: return os << "GREATER";
    case TokenType::SEMICOLON: return os << "SEMICOLON";
    case TokenType::IDENTIFIER: return os << "IDENTIFIER";
    case TokenType::STRING_LITERAL: return os << "STRING_LITERAL";
    case TokenType::INT_LITERAL: return os << "INT_LITERAL";
    case TokenType::FLOAT_LITERAL: return os << "FLOAT_LITERAL";
    case TokenType::END_OF_FILE: return os << "EOF";
    default: return os << "UNKNOWN";
    }
}
#include "lexer.h"
#include <cctype>
#include <unordered_map>

namespace carch {
namespace lexer {

Lexer::Lexer(const std::string& source)
    : source_(source), position_(0), peek_position_(0), line_(1), column_(1) {}

Lexer::Lexer(std::string&& source)
    : source_(std::move(source)), position_(0), peek_position_(0), line_(1), column_(1) {}

Token Lexer::next_token() {
    if (peeked_token_.has_value()) {
        Token token = std::move(peeked_token_.value());
        peeked_token_.reset();
        return token;
    }
    
    return scan_token();
}

Token Lexer::peek_token() {
    if (!peeked_token_.has_value()) {
        peeked_token_ = scan_token();
    }
    return peeked_token_.value();
}

bool Lexer::has_more_tokens() const {
    return !is_at_end();
}

char Lexer::current_char() const {
    if (is_at_end()) return '\0';
    return source_[position_];
}

char Lexer::peek_char(size_t offset) const {
    size_t pos = position_ + offset;
    if (pos >= source_.size()) return '\0';
    return source_[pos];
}

void Lexer::advance() {
    if (is_at_end()) return;
    
    if (source_[position_] == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    position_++;
}

bool Lexer::is_at_end() const {
    return position_ >= source_.size();
}

Token Lexer::scan_token() {
    // Skip whitespace (except newlines which may be significant)
    while (!is_at_end() && is_whitespace(current_char()) && current_char() != '\n') {
        advance();
    }
    
    if (is_at_end()) {
        return Token(TokenType::END_OF_FILE, "", line_, column_);
    }
    
    uint32_t token_line = line_;
    uint32_t token_column = column_;
    char c = current_char();
    
    // Newlines
    if (c == '\n') {
        advance();
        return Token(TokenType::NEWLINE, "\n", token_line, token_column);
    }
    
    // Comments
    if (c == '/' && peek_char() == '/') {
        return scan_single_line_comment();
    }
    if (c == '/' && peek_char() == '*') {
        return scan_multi_line_comment();
    }
    
    // Symbols
    if (c == ':') { advance(); return Token(TokenType::COLON, ":", token_line, token_column); }
    if (c == ',') { advance(); return Token(TokenType::COMMA, ",", token_line, token_column); }
    if (c == '{') { advance(); return Token(TokenType::LBRACE, "{", token_line, token_column); }
    if (c == '}') { advance(); return Token(TokenType::RBRACE, "}", token_line, token_column); }
    if (c == '<') { advance(); return Token(TokenType::LANGLE, "<", token_line, token_column); }
    if (c == '>') { advance(); return Token(TokenType::RANGLE, ">", token_line, token_column); }
    if (c == '(') { advance(); return Token(TokenType::LPAREN, "(", token_line, token_column); }
    if (c == ')') { advance(); return Token(TokenType::RPAREN, ")", token_line, token_column); }
    
    // String literals
    if (c == '"') {
        return scan_string();
    }
    
    // Numbers
    if (is_digit(c) || (c == '-' && is_digit(peek_char()))) {
        return scan_number();
    }
    
    // Identifiers and keywords
    if (is_letter(c) || c == '_') {
        return scan_identifier_or_keyword();
    }
    
    // Unknown character
    std::string error = "Unexpected character: '";
    error += c;
    error += "'";
    advance();
    return make_error_token(error);
}

Token Lexer::scan_identifier_or_keyword() {
    uint32_t token_line = line_;
    uint32_t token_column = column_;
    std::string lexeme;
    
    while (!is_at_end() && (is_letter(current_char()) || is_digit(current_char()) || current_char() == '_')) {
        lexeme += current_char();
        advance();
    }
    
    TokenType type = identify_keyword(lexeme);
    return Token(type, std::move(lexeme), token_line, token_column);
}

Token Lexer::scan_number() {
    uint32_t token_line = line_;
    uint32_t token_column = column_;
    std::string lexeme;
    
    // Handle negative sign
    if (current_char() == '-') {
        lexeme += current_char();
        advance();
    }
    
    // Hex, binary, octal literals
    if (current_char() == '0' && !is_at_end()) {
        char next = peek_char();
        if (next == 'x' || next == 'X') {
            lexeme += current_char(); advance();
            lexeme += current_char(); advance();
            while (!is_at_end() && is_hex_digit(current_char())) {
                lexeme += current_char();
                advance();
            }
            return Token(TokenType::NUMBER_LITERAL, std::move(lexeme), token_line, token_column);
        } else if (next == 'b' || next == 'B') {
            lexeme += current_char(); advance();
            lexeme += current_char(); advance();
            while (!is_at_end() && (current_char() == '0' || current_char() == '1')) {
                lexeme += current_char();
                advance();
            }
            return Token(TokenType::NUMBER_LITERAL, std::move(lexeme), token_line, token_column);
        } else if (next == 'o' || next == 'O') {
            lexeme += current_char(); advance();
            lexeme += current_char(); advance();
            while (!is_at_end() && current_char() >= '0' && current_char() <= '7') {
                lexeme += current_char();
                advance();
            }
            return Token(TokenType::NUMBER_LITERAL, std::move(lexeme), token_line, token_column);
        }
    }
    
    // Decimal integer part
    while (!is_at_end() && is_digit(current_char())) {
        lexeme += current_char();
        advance();
    }
    
    // Decimal point (float)
    if (!is_at_end() && current_char() == '.' && is_digit(peek_char())) {
        lexeme += current_char();
        advance();
        while (!is_at_end() && is_digit(current_char())) {
            lexeme += current_char();
            advance();
        }
    }
    
    // Exponent
    if (!is_at_end() && (current_char() == 'e' || current_char() == 'E')) {
        lexeme += current_char();
        advance();
        if (!is_at_end() && (current_char() == '+' || current_char() == '-')) {
            lexeme += current_char();
            advance();
        }
        while (!is_at_end() && is_digit(current_char())) {
            lexeme += current_char();
            advance();
        }
    }
    
    return Token(TokenType::NUMBER_LITERAL, std::move(lexeme), token_line, token_column);
}

Token Lexer::scan_string() {
    uint32_t token_line = line_;
    uint32_t token_column = column_;
    std::string lexeme;
    
    // Skip opening quote
    advance();
    
    while (!is_at_end() && current_char() != '"') {
        if (current_char() == '\\') {
            advance();
            if (is_at_end()) {
                return make_error_token("Unterminated string literal");
            }
            
            char escape = current_char();
            switch (escape) {
                case 'n': lexeme += '\n'; break;
                case 't': lexeme += '\t'; break;
                case 'r': lexeme += '\r'; break;
                case '\\': lexeme += '\\'; break;
                case '"': lexeme += '"'; break;
                case '\'': lexeme += '\''; break;
                case '0': lexeme += '\0'; break;
                case 'x': {
                    // Hex escape \xHH
                    advance();
                    if (is_at_end() || !is_hex_digit(current_char())) {
                        return make_error_token("Invalid hex escape sequence: missing first hex digit");
                    }
                    int hex_value = 0;
                    // First hex digit
                    char first = current_char();
                    if (first >= '0' && first <= '9') hex_value = (first - '0') << 4;
                    else if (first >= 'a' && first <= 'f') hex_value = (first - 'a' + 10) << 4;
                    else if (first >= 'A' && first <= 'F') hex_value = (first - 'A' + 10) << 4;
                    advance();
                    
                    if (is_at_end() || !is_hex_digit(current_char())) {
                        return make_error_token("Invalid hex escape sequence: missing second hex digit");
                    }
                    // Second hex digit
                    char second = current_char();
                    if (second >= '0' && second <= '9') hex_value |= (second - '0');
                    else if (second >= 'a' && second <= 'f') hex_value |= (second - 'a' + 10);
                    else if (second >= 'A' && second <= 'F') hex_value |= (second - 'A' + 10);
                    
                    lexeme += static_cast<char>(hex_value);
                    advance();
                    continue;
                }
                default:
                    lexeme += escape;
                    break;
            }
            advance();
        } else {
            lexeme += current_char();
            advance();
        }
    }
    
    if (is_at_end()) {
        return make_error_token("Unterminated string literal");
    }
    
    // Skip closing quote
    advance();
    
    return Token(TokenType::STRING_LITERAL, std::move(lexeme), token_line, token_column);
}

Token Lexer::scan_single_line_comment() {
    uint32_t token_line = line_;
    uint32_t token_column = column_;
    std::string lexeme;
    
    // Skip //
    advance(); advance();
    
    while (!is_at_end() && current_char() != '\n') {
        lexeme += current_char();
        advance();
    }
    
    return Token(TokenType::COMMENT, std::move(lexeme), token_line, token_column);
}

Token Lexer::scan_multi_line_comment() {
    uint32_t token_line = line_;
    uint32_t token_column = column_;
    std::string lexeme;
    
    // Skip /*
    advance(); advance();
    
    while (!is_at_end()) {
        if (current_char() == '*' && peek_char() == '/') {
            advance(); advance();
            return Token(TokenType::COMMENT, std::move(lexeme), token_line, token_column);
        }
        lexeme += current_char();
        advance();
    }
    
    return make_error_token("Unterminated multi-line comment");
}

bool Lexer::is_letter(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Lexer::is_digit(char c) const {
    return c >= '0' && c <= '9';
}

bool Lexer::is_hex_digit(char c) const {
    return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool Lexer::is_whitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

TokenType Lexer::identify_keyword(const std::string& word) const {
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"struct", TokenType::STRUCT},
        {"variant", TokenType::VARIANT},
        {"enum", TokenType::ENUM},
        {"unit", TokenType::UNIT},
        {"array", TokenType::ARRAY},
        {"map", TokenType::MAP},
        {"optional", TokenType::OPTIONAL},
        {"ref", TokenType::REF},
        {"entity", TokenType::ENTITY},
        {"str", TokenType::STR},
        {"int", TokenType::INT},
        {"u8", TokenType::U8},
        {"u16", TokenType::U16},
        {"u32", TokenType::U32},
        {"u64", TokenType::U64},
        {"i8", TokenType::I8},
        {"i16", TokenType::I16},
        {"i32", TokenType::I32},
        {"i64", TokenType::I64},
        {"f32", TokenType::F32},
        {"f64", TokenType::F64},
        {"bool", TokenType::BOOL},
        {"true", TokenType::TRUE},
        {"false", TokenType::FALSE}
    };
    
    auto it = keywords.find(word);
    if (it != keywords.end()) {
        return it->second;
    }
    return TokenType::IDENTIFIER;
}

void Lexer::report_error(const std::string& message) {
    std::string error = "Line " + std::to_string(line_) + ", Column " + std::to_string(column_) + ": " + message;
    errors_.push_back(error);
}

Token Lexer::make_error_token(const std::string& message) {
    report_error(message);
    return Token(message, line_, column_);
}

} // namespace lexer
} // namespace carch

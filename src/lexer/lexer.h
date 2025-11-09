#pragma once

#include "token.h"
#include <string>
#include <vector>
#include <optional>

namespace carch {
namespace lexer {

class Lexer {
public:
    explicit Lexer(const std::string& source);
    explicit Lexer(std::string&& source);
    
    // Tokenization
    Token next_token();
    Token peek_token();
    bool has_more_tokens() const;
    
    // Position tracking
    uint32_t current_line() const { return line_; }
    uint32_t current_column() const { return column_; }
    
    // Error reporting
    const std::vector<std::string>& errors() const { return errors_; }
    bool has_errors() const { return !errors_.empty(); }

private:
    std::string source_;
    size_t position_;
    size_t peek_position_;
    uint32_t line_;
    uint32_t column_;
    std::vector<std::string> errors_;
    std::optional<Token> peeked_token_;
    
    // Character operations
    char current_char() const;
    char peek_char(size_t offset = 1) const;
    void advance();
    bool is_at_end() const;
    
    // Token scanning
    Token scan_token();
    Token scan_identifier_or_keyword();
    Token scan_number();
    Token scan_string();
    Token scan_single_line_comment();
    Token scan_multi_line_comment();
    
    // Character classification
    bool is_letter(char c) const;
    bool is_digit(char c) const;
    bool is_hex_digit(char c) const;
    bool is_whitespace(char c) const;
    
    // Keyword recognition
    TokenType identify_keyword(const std::string& word) const;
    
    // Error handling
    void report_error(const std::string& message);
    Token make_error_token(const std::string& message);
};

} // namespace lexer
} // namespace carch

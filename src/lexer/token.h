#pragma once

#include <string>
#include <cstdint>

namespace carch {
namespace lexer {

enum class TokenType {
    // Keywords
    STRUCT,
    VARIANT,
    ENUM,
    UNIT,
    
    // Container types
    ARRAY,
    MAP,
    OPTIONAL,
    REF,
    ENTITY,
    
    // Primitive types
    STR,
    INT,
    U8, U16, U32, U64,
    I8, I16, I32, I64,
    F32, F64,
    BOOL,
    
    // Symbols
    COLON,          // :
    COMMA,          // ,
    LBRACE,         // {
    RBRACE,         // }
    LANGLE,         // <
    RANGLE,         // >
    LPAREN,         // (
    RPAREN,         // )
    
    // Identifiers and literals
    IDENTIFIER,
    STRING_LITERAL,
    NUMBER_LITERAL,
    TRUE,
    FALSE,
    
    // Special tokens
    NEWLINE,
    WHITESPACE,
    COMMENT,
    END_OF_FILE,
    ERROR
};

struct Token {
    TokenType type;
    std::string lexeme;
    uint32_t line;
    uint32_t column;
    std::string error_message;  // For ERROR tokens
    
    Token(TokenType t, const std::string& lex, uint32_t ln, uint32_t col)
        : type(t), lexeme(lex), line(ln), column(col) {}
    
    Token(TokenType t, std::string&& lex, uint32_t ln, uint32_t col)
        : type(t), lexeme(std::move(lex)), line(ln), column(col) {}
    
    // For error tokens
    Token(const std::string& error, uint32_t ln, uint32_t col)
        : type(TokenType::ERROR), lexeme(""), line(ln), column(col), error_message(error) {}
    
    bool is_keyword() const;
    bool is_primitive_type() const;
    bool is_symbol() const;
    bool is_literal() const;
    
    std::string to_string() const;
};

const char* token_type_to_string(TokenType type);

} // namespace lexer
} // namespace carch

#include "lexer/token.h"

namespace carch {
namespace lexer {

bool Token::is_keyword() const {
    return type == TokenType::STRUCT ||
           type == TokenType::VARIANT ||
           type == TokenType::ENUM ||
           type == TokenType::UNIT ||
           type == TokenType::ARRAY ||
           type == TokenType::MAP ||
           type == TokenType::OPTIONAL ||
           type == TokenType::REF ||
           type == TokenType::ENTITY;
}

bool Token::is_primitive_type() const {
    return type == TokenType::STR ||
           type == TokenType::INT ||
           type == TokenType::BOOL ||
           type == TokenType::U8 || type == TokenType::U16 ||
           type == TokenType::U32 || type == TokenType::U64 ||
           type == TokenType::I8 || type == TokenType::I16 ||
           type == TokenType::I32 || type == TokenType::I64 ||
           type == TokenType::F32 || type == TokenType::F64;
}

bool Token::is_symbol() const {
    return type == TokenType::COLON ||
           type == TokenType::COMMA ||
           type == TokenType::LBRACE ||
           type == TokenType::RBRACE ||
           type == TokenType::LANGLE ||
           type == TokenType::RANGLE ||
           type == TokenType::LPAREN ||
           type == TokenType::RPAREN;
}

bool Token::is_literal() const {
    return type == TokenType::STRING_LITERAL ||
           type == TokenType::NUMBER_LITERAL ||
           type == TokenType::TRUE ||
           type == TokenType::FALSE;
}

std::string Token::to_string() const {
    std::string result = token_type_to_string(type);
    if (!lexeme.empty()) {
        result += " '" + lexeme + "'";
    }
    result += " at Line " + std::to_string(line) + ", Column " + std::to_string(column);
    return result;
}

const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TokenType::STRUCT: return "STRUCT";
        case TokenType::VARIANT: return "VARIANT";
        case TokenType::ENUM: return "ENUM";
        case TokenType::UNIT: return "UNIT";
        case TokenType::ARRAY: return "ARRAY";
        case TokenType::MAP: return "MAP";
        case TokenType::OPTIONAL: return "OPTIONAL";
        case TokenType::REF: return "REF";
        case TokenType::ENTITY: return "ENTITY";
        case TokenType::STR: return "STR";
        case TokenType::INT: return "INT";
        case TokenType::U8: return "U8";
        case TokenType::U16: return "U16";
        case TokenType::U32: return "U32";
        case TokenType::U64: return "U64";
        case TokenType::I8: return "I8";
        case TokenType::I16: return "I16";
        case TokenType::I32: return "I32";
        case TokenType::I64: return "I64";
        case TokenType::F32: return "F32";
        case TokenType::F64: return "F64";
        case TokenType::BOOL: return "BOOL";
        case TokenType::COLON: return "COLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::LANGLE: return "LANGLE";
        case TokenType::RANGLE: return "RANGLE";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::NUMBER_LITERAL: return "NUMBER_LITERAL";
        case TokenType::TRUE: return "TRUE";
        case TokenType::FALSE: return "FALSE";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::COMMENT: return "COMMENT";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

} // namespace lexer
} // namespace carch

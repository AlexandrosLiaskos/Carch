#include "parser.h"
#include <sstream>

namespace carch {
namespace parser {

Parser::Parser(lexer::Lexer& lexer)
    : lexer_(lexer), current_token_(lexer::TokenType::END_OF_FILE, "", 0, 0) {
    advance();  // Load first token
}

std::unique_ptr<SchemaNode> Parser::parse() {
    return parse_schema();
}

void Parser::advance() {
    do {
        current_token_ = lexer_.next_token();
    } while (current_token_.type == lexer::TokenType::COMMENT || 
             current_token_.type == lexer::TokenType::WHITESPACE ||
             current_token_.type == lexer::TokenType::NEWLINE);
}

bool Parser::match(lexer::TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(lexer::TokenType type) const {
    return current_token_.type == type;
}

lexer::Token Parser::expect(lexer::TokenType type, const std::string& message) {
    if (check(type)) {
        lexer::Token token = current_token_;
        advance();
        return token;
    }
    
    report_error(message);
    return current_token_;
}

void Parser::skip_newlines() {
    while (match(lexer::TokenType::NEWLINE)) {
        // Skip
    }
}

void Parser::synchronize() {
    advance();
    
    while (!check(lexer::TokenType::END_OF_FILE)) {
        if (current_token_.type == lexer::TokenType::NEWLINE) {
            advance();
            return;
        }
        
        if (check(lexer::TokenType::IDENTIFIER)) {
            return;
        }
        
        advance();
    }
}

std::unique_ptr<SchemaNode> Parser::parse_schema() {
    auto schema = std::make_unique<SchemaNode>(current_token_.line, current_token_.column);
    
    skip_newlines();
    
    while (!check(lexer::TokenType::END_OF_FILE)) {
        auto def = parse_type_definition();
        if (def) {
            schema->definitions.push_back(std::move(def));
        } else {
            synchronize();
        }
        skip_newlines();
    }
    
    return schema;
}

std::unique_ptr<TypeDefinitionNode> Parser::parse_type_definition() {
    if (!check(lexer::TokenType::IDENTIFIER)) {
        report_error("Expected type name");
        return nullptr;
    }
    
    lexer::Token name_token = current_token_;
    advance();
    
    expect(lexer::TokenType::COLON, "Expected ':' after type name");
    
    auto type_expr = parse_type_expr();
    if (!type_expr) {
        return nullptr;
    }
    
    auto def = std::make_unique<TypeDefinitionNode>(name_token.lexeme, name_token.line, name_token.column);
    def->type = std::move(type_expr);
    
    return def;
}

std::unique_ptr<TypeExprNode> Parser::parse_type_expr() {
    if (check(lexer::TokenType::STRUCT)) {
        return parse_struct_type();
    } else if (check(lexer::TokenType::VARIANT)) {
        return parse_variant_type();
    } else if (check(lexer::TokenType::ENUM)) {
        return parse_enum_type();
    } else if (check(lexer::TokenType::ARRAY) || check(lexer::TokenType::MAP) || check(lexer::TokenType::OPTIONAL)) {
        return parse_container_type();
    } else if (check(lexer::TokenType::REF)) {
        return parse_ref_type();
    } else if (is_primitive_type()) {
        return parse_primitive_type();
    } else if (check(lexer::TokenType::IDENTIFIER)) {
        auto node = std::make_unique<IdentifierTypeNode>(current_token_.lexeme, current_token_.line, current_token_.column);
        advance();
        return node;
    }
    
    report_error("Expected type expression");
    return nullptr;
}

std::unique_ptr<StructTypeNode> Parser::parse_struct_type() {
    lexer::Token start = current_token_;
    expect(lexer::TokenType::STRUCT, "Expected 'struct'");
    expect(lexer::TokenType::LBRACE, "Expected '{' after 'struct'");
    
    auto struct_node = std::make_unique<StructTypeNode>(start.line, start.column);
    
    skip_newlines();
    
    // Parse fields
    while (!check(lexer::TokenType::RBRACE) && !check(lexer::TokenType::END_OF_FILE)) {
        auto field = parse_field();
        if (field) {
            struct_node->fields.push_back(std::move(field));
        }
        
        skip_newlines();
        
        if (match(lexer::TokenType::COMMA)) {
            skip_newlines();
        } else if (!check(lexer::TokenType::RBRACE)) {
            // In compact syntax, fields might not be separated by newlines
            if (!check(lexer::TokenType::RBRACE)) {
                break;
            }
        }
    }
    
    expect(lexer::TokenType::RBRACE, "Expected '}' after struct fields");
    
    return struct_node;
}

std::unique_ptr<VariantTypeNode> Parser::parse_variant_type() {
    lexer::Token start = current_token_;
    expect(lexer::TokenType::VARIANT, "Expected 'variant'");
    expect(lexer::TokenType::LBRACE, "Expected '{' after 'variant'");
    
    auto variant_node = std::make_unique<VariantTypeNode>(start.line, start.column);
    
    skip_newlines();
    
    // Parse alternatives
    while (!check(lexer::TokenType::RBRACE) && !check(lexer::TokenType::END_OF_FILE)) {
        auto alt = parse_alternative();
        if (alt) {
            variant_node->alternatives.push_back(std::move(alt));
        }
        
        skip_newlines();
        
        if (match(lexer::TokenType::COMMA)) {
            skip_newlines();
        } else if (!check(lexer::TokenType::RBRACE)) {
            break;
        }
    }
    
    expect(lexer::TokenType::RBRACE, "Expected '}' after variant alternatives");
    
    return variant_node;
}

std::unique_ptr<EnumTypeNode> Parser::parse_enum_type() {
    lexer::Token start = current_token_;
    expect(lexer::TokenType::ENUM, "Expected 'enum'");
    expect(lexer::TokenType::LBRACE, "Expected '{' after 'enum'");
    
    auto enum_node = std::make_unique<EnumTypeNode>(start.line, start.column);
    
    skip_newlines();
    
    // Parse enum values
    while (!check(lexer::TokenType::RBRACE) && !check(lexer::TokenType::END_OF_FILE)) {
        if (!check(lexer::TokenType::IDENTIFIER)) {
            report_error("Expected enum value");
            break;
        }
        
        enum_node->values.push_back(current_token_.lexeme);
        advance();
        
        skip_newlines();
        
        if (match(lexer::TokenType::COMMA)) {
            skip_newlines();
        } else if (!check(lexer::TokenType::RBRACE)) {
            break;
        }
    }
    
    expect(lexer::TokenType::RBRACE, "Expected '}' after enum values");
    
    return enum_node;
}

std::unique_ptr<FieldNode> Parser::parse_field() {
    if (!check(lexer::TokenType::IDENTIFIER)) {
        report_error("Expected field name");
        return nullptr;
    }
    
    lexer::Token name_token = current_token_;
    advance();
    
    expect(lexer::TokenType::COLON, "Expected ':' after field name");
    
    auto type_expr = parse_type_expr();
    if (!type_expr) {
        return nullptr;
    }
    
    auto field = std::make_unique<FieldNode>(name_token.lexeme, name_token.line, name_token.column);
    field->type = std::move(type_expr);
    
    return field;
}

std::unique_ptr<AlternativeNode> Parser::parse_alternative() {
    if (!check(lexer::TokenType::IDENTIFIER)) {
        report_error("Expected alternative name");
        return nullptr;
    }
    
    lexer::Token name_token = current_token_;
    advance();
    
    auto alt = std::make_unique<AlternativeNode>(name_token.lexeme, name_token.line, name_token.column);
    
    // Check for explicit type
    if (match(lexer::TokenType::COLON)) {
        alt->type = parse_type_expr();
    }
    // Otherwise, type is implicitly unit (nullptr)
    
    return alt;
}

std::unique_ptr<TypeExprNode> Parser::parse_primitive_type() {
    if (!is_primitive_type()) {
        report_error("Expected primitive type");
        return nullptr;
    }
    
    PrimitiveType prim = token_to_primitive_type(current_token_.type);
    auto node = std::make_unique<PrimitiveTypeNode>(prim, current_token_.line, current_token_.column);
    advance();
    
    return node;
}

std::unique_ptr<TypeExprNode> Parser::parse_container_type() {
    lexer::Token start = current_token_;
    ContainerKind kind;
    
    if (match(lexer::TokenType::ARRAY)) {
        kind = ContainerKind::ARRAY;
    } else if (match(lexer::TokenType::MAP)) {
        kind = ContainerKind::MAP;
    } else if (match(lexer::TokenType::OPTIONAL)) {
        kind = ContainerKind::OPTIONAL;
    } else {
        report_error("Expected container type");
        return nullptr;
    }
    
    expect(lexer::TokenType::LANGLE, "Expected '<' after container type");
    
    auto container = std::make_unique<ContainerTypeNode>(kind, start.line, start.column);
    
    if (kind == ContainerKind::MAP) {
        container->key_type = parse_type_expr();
        expect(lexer::TokenType::COMMA, "Expected ',' between map key and value types");
        container->value_type = parse_type_expr();
    } else {
        container->element_type = parse_type_expr();
    }
    
    expect(lexer::TokenType::RANGLE, "Expected '>' after container type parameter");
    
    return container;
}

std::unique_ptr<TypeExprNode> Parser::parse_ref_type() {
    lexer::Token start = current_token_;
    expect(lexer::TokenType::REF, "Expected 'ref'");
    expect(lexer::TokenType::LANGLE, "Expected '<' after 'ref'");
    expect(lexer::TokenType::ENTITY, "Expected 'entity' in ref type");
    expect(lexer::TokenType::RANGLE, "Expected '>' after 'entity'");
    
    return std::make_unique<RefTypeNode>(start.line, start.column);
}

bool Parser::is_type_start() const {
    return check(lexer::TokenType::STRUCT) ||
           check(lexer::TokenType::VARIANT) ||
           check(lexer::TokenType::ENUM) ||
           check(lexer::TokenType::ARRAY) ||
           check(lexer::TokenType::MAP) ||
           check(lexer::TokenType::OPTIONAL) ||
           check(lexer::TokenType::REF) ||
           is_primitive_type() ||
           check(lexer::TokenType::IDENTIFIER);
}

bool Parser::is_primitive_type() const {
    return check(lexer::TokenType::STR) ||
           check(lexer::TokenType::INT) ||
           check(lexer::TokenType::BOOL) ||
           check(lexer::TokenType::UNIT) ||
           check(lexer::TokenType::U8) || check(lexer::TokenType::U16) ||
           check(lexer::TokenType::U32) || check(lexer::TokenType::U64) ||
           check(lexer::TokenType::I8) || check(lexer::TokenType::I16) ||
           check(lexer::TokenType::I32) || check(lexer::TokenType::I64) ||
           check(lexer::TokenType::F32) || check(lexer::TokenType::F64);
}

PrimitiveType Parser::token_to_primitive_type(lexer::TokenType type) const {
    switch (type) {
        case lexer::TokenType::STR: return PrimitiveType::STR;
        case lexer::TokenType::INT: return PrimitiveType::INT;
        case lexer::TokenType::BOOL: return PrimitiveType::BOOL;
        case lexer::TokenType::UNIT: return PrimitiveType::UNIT;
        case lexer::TokenType::U8: return PrimitiveType::U8;
        case lexer::TokenType::U16: return PrimitiveType::U16;
        case lexer::TokenType::U32: return PrimitiveType::U32;
        case lexer::TokenType::U64: return PrimitiveType::U64;
        case lexer::TokenType::I8: return PrimitiveType::I8;
        case lexer::TokenType::I16: return PrimitiveType::I16;
        case lexer::TokenType::I32: return PrimitiveType::I32;
        case lexer::TokenType::I64: return PrimitiveType::I64;
        case lexer::TokenType::F32: return PrimitiveType::F32;
        case lexer::TokenType::F64: return PrimitiveType::F64;
        default: return PrimitiveType::INT;
    }
}

void Parser::report_error(const std::string& message) {
    report_error(message, current_token_.line, current_token_.column);
}

void Parser::report_error(const std::string& message, uint32_t line, uint32_t column) {
    std::ostringstream oss;
    oss << "Line " << line << ", Column " << column << ": " << message;
    errors_.push_back(oss.str());
}

} // namespace parser
} // namespace carch

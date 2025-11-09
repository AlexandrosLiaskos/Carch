#pragma once

#include "ast.h"
#include "../lexer/lexer.h"
#include <memory>
#include <vector>
#include <string>

namespace carch {
namespace parser {

class Parser {
public:
    explicit Parser(lexer::Lexer& lexer);
    
    // Main parsing entry point
    std::unique_ptr<SchemaNode> parse();
    
    // Error reporting
    const std::vector<std::string>& errors() const { return errors_; }
    bool has_errors() const { return !errors_.empty(); }

private:
    lexer::Lexer& lexer_;
    lexer::Token current_token_;
    std::vector<std::string> errors_;
    
    // Token operations
    void advance();
    bool match(lexer::TokenType type);
    bool check(lexer::TokenType type) const;
    lexer::Token expect(lexer::TokenType type, const std::string& message);
    void skip_newlines();
    void synchronize();
    
    // Parsing methods
    std::unique_ptr<SchemaNode> parse_schema();
    std::unique_ptr<TypeDefinitionNode> parse_type_definition();
    std::unique_ptr<TypeExprNode> parse_type_expr();
    std::unique_ptr<StructTypeNode> parse_struct_type();
    std::unique_ptr<VariantTypeNode> parse_variant_type();
    std::unique_ptr<EnumTypeNode> parse_enum_type();
    std::unique_ptr<FieldNode> parse_field();
    std::unique_ptr<AlternativeNode> parse_alternative();
    std::unique_ptr<TypeExprNode> parse_primitive_type();
    std::unique_ptr<TypeExprNode> parse_container_type();
    std::unique_ptr<TypeExprNode> parse_ref_type();
    
    // Helper methods
    bool is_type_start() const;
    bool is_primitive_type() const;
    PrimitiveType token_to_primitive_type(lexer::TokenType type) const;
    
    // Error handling
    void report_error(const std::string& message);
    void report_error(const std::string& message, uint32_t line, uint32_t column);
};

} // namespace parser
} // namespace carch

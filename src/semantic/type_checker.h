#pragma once

#include "../parser/ast.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <memory>

namespace carch {
namespace semantic {

class TypeChecker {
public:
    explicit TypeChecker(parser::SchemaNode* schema);
    
    // Main semantic analysis entry point
    bool check();
    
    // Error reporting
    const std::vector<std::string>& errors() const { return errors_; }
    bool has_errors() const { return !errors_.empty(); }

private:
    parser::SchemaNode* schema_;
    std::vector<std::string> errors_;
    
    // Symbol table: type name -> type definition
    std::unordered_map<std::string, parser::TypeDefinitionNode*> symbol_table_;
    
    // Definition order tracking: type name -> definition index
    std::unordered_map<std::string, size_t> definition_order_;
    size_t current_definition_index_;
    
    // For circular dependency detection
    std::unordered_set<std::string> visiting_;
    std::unordered_set<std::string> visited_;
    
    // Validation methods
    void build_symbol_table();
    void check_type_definitions();
    void check_type_definition(parser::TypeDefinitionNode* def);
    void check_struct_type(parser::StructTypeNode* node, const std::string& context);
    void check_variant_type(parser::VariantTypeNode* node, const std::string& context);
    void check_enum_type(parser::EnumTypeNode* node, const std::string& context);
    void check_container_type(parser::ContainerTypeNode* node, const std::string& context);
    void check_type_expr(parser::TypeExprNode* expr, const std::string& context);
    
    // Check for circular dependencies
    bool has_circular_dependency(const std::string& type_name);
    bool check_circular_in_type_expr(parser::TypeExprNode* expr, const std::string& current_type);
    
    // Type reference validation
    bool is_type_defined(const std::string& type_name) const;
    bool is_primitive_type(parser::TypeExprNode* expr) const;
    bool is_leaf_type(parser::TypeExprNode* expr) const;
    void check_leaf_nodes(parser::TypeExprNode* expr, const std::string& context, bool must_terminate);
    
    // Error reporting
    void report_error(const std::string& message);
    void report_error(const std::string& message, uint32_t line, uint32_t column);
    void report_error(const std::string& message, parser::ASTNode* node);
};

} // namespace semantic
} // namespace carch

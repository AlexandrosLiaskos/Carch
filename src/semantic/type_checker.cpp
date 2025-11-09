#include "type_checker.h"
#include <sstream>

namespace carch {
namespace semantic {

TypeChecker::TypeChecker(parser::SchemaNode* schema)
    : schema_(schema), current_definition_index_(0) {}

bool TypeChecker::check() {
    errors_.clear();
    symbol_table_.clear();
    visiting_.clear();
    visited_.clear();
    
    // Phase 1: Build symbol table
    build_symbol_table();
    
    if (has_errors()) {
        return false;
    }
    
    // Phase 2: Check type definitions
    check_type_definitions();
    
    return !has_errors();
}

void TypeChecker::build_symbol_table() {
    size_t index = 0;
    for (auto& def : schema_->definitions) {
        if (symbol_table_.count(def->name) > 0) {
            report_error("Duplicate type definition: '" + def->name + "'", def.get());
        } else {
            symbol_table_[def->name] = def.get();
            definition_order_[def->name] = index;
        }
        index++;
    }
}

void TypeChecker::check_type_definitions() {
    current_definition_index_ = 0;
    for (auto& def : schema_->definitions) {
        check_type_definition(def.get());
        current_definition_index_++;
    }
    
    // Check for circular dependencies
    for (auto& def : schema_->definitions) {
        if (has_circular_dependency(def->name)) {
            report_error("Circular type dependency detected for: '" + def->name + "'", def.get());
        }
    }
}

void TypeChecker::check_type_definition(parser::TypeDefinitionNode* def) {
    check_type_expr(def->type.get(), def->name);
    // Check that all paths terminate at leaf types
    check_leaf_nodes(def->type.get(), def->name, false);
}

void TypeChecker::check_struct_type(parser::StructTypeNode* node, const std::string& context) {
    if (node->fields.empty()) {
        report_error("Struct must have at least one field in type '" + context + "'", node);
    }
    
    // Check field name uniqueness
    std::unordered_set<std::string> field_names;
    for (auto& field : node->fields) {
        if (field_names.count(field->name) > 0) {
            report_error("Duplicate field name '" + field->name + "' in struct in type '" + context + "'", field.get());
        } else {
            field_names.insert(field->name);
        }
        
        // Check field type
        check_type_expr(field->type.get(), context + "." + field->name);
    }
}

void TypeChecker::check_variant_type(parser::VariantTypeNode* node, const std::string& context) {
    if (node->alternatives.empty()) {
        report_error("Variant must have at least one alternative in type '" + context + "'", node);
    }
    
    // Check alternative name uniqueness
    std::unordered_set<std::string> alt_names;
    for (auto& alt : node->alternatives) {
        if (alt_names.count(alt->name) > 0) {
            report_error("Duplicate alternative name '" + alt->name + "' in variant in type '" + context + "'", alt.get());
        } else {
            alt_names.insert(alt->name);
        }
        
        // Check alternative type (if not unit/implicit)
        if (alt->type) {
            check_type_expr(alt->type.get(), context + "." + alt->name);
        }
    }
}

void TypeChecker::check_enum_type(parser::EnumTypeNode* node, const std::string& context) {
    if (node->values.empty()) {
        report_error("Enum must have at least one value in type '" + context + "'", node);
    }
    
    // Check value uniqueness
    std::unordered_set<std::string> value_set;
    for (const auto& value : node->values) {
        if (value_set.count(value) > 0) {
            report_error("Duplicate enum value '" + value + "' in type '" + context + "'", node);
        } else {
            value_set.insert(value);
        }
    }
}

void TypeChecker::check_container_type(parser::ContainerTypeNode* node, const std::string& context) {
    if (node->kind == parser::ContainerKind::ARRAY || node->kind == parser::ContainerKind::OPTIONAL) {
        if (!node->element_type) {
            report_error("Container type missing element type in '" + context + "'", node);
            return;
        }
        check_type_expr(node->element_type.get(), context);
        
        // Check for optional<optional<T>>
        if (node->kind == parser::ContainerKind::OPTIONAL) {
            auto* elem_as_container = dynamic_cast<parser::ContainerTypeNode*>(node->element_type.get());
            if (elem_as_container && elem_as_container->kind == parser::ContainerKind::OPTIONAL) {
                report_error("Nested optional types (optional<optional<T>>) are not allowed in '" + context + "'", node);
            }
        }
    } else if (node->kind == parser::ContainerKind::MAP) {
        if (!node->key_type || !node->value_type) {
            report_error("Map type missing key or value type in '" + context + "'", node);
            return;
        }
        check_type_expr(node->key_type.get(), context + " (map key)");
        check_type_expr(node->value_type.get(), context + " (map value)");
    }
}

void TypeChecker::check_type_expr(parser::TypeExprNode* expr, const std::string& context) {
    if (auto* struct_type = dynamic_cast<parser::StructTypeNode*>(expr)) {
        check_struct_type(struct_type, context);
    } else if (auto* variant_type = dynamic_cast<parser::VariantTypeNode*>(expr)) {
        check_variant_type(variant_type, context);
    } else if (auto* enum_type = dynamic_cast<parser::EnumTypeNode*>(expr)) {
        check_enum_type(enum_type, context);
    } else if (auto* container_type = dynamic_cast<parser::ContainerTypeNode*>(expr)) {
        check_container_type(container_type, context);
    } else if (auto* id_type = dynamic_cast<parser::IdentifierTypeNode*>(expr)) {
        if (!is_type_defined(id_type->name)) {
            report_error("Undefined type '" + id_type->name + "' referenced in '" + context + "'", expr);
        } else {
            // Check for forward references
            auto ref_order_it = definition_order_.find(id_type->name);
            if (ref_order_it != definition_order_.end() && 
                ref_order_it->second > current_definition_index_) {
                report_error("Forward reference to type '" + id_type->name + "' (defined later) in '" + context + "'", expr);
            }
        }
    }
    // Primitive and ref types are always valid
}

bool TypeChecker::has_circular_dependency(const std::string& type_name) {
    visiting_.clear();
    visited_.clear();
    
    auto it = symbol_table_.find(type_name);
    if (it == symbol_table_.end()) {
        return false;
    }
    
    visiting_.insert(type_name);
    bool has_cycle = check_circular_in_type_expr(it->second->type.get(), type_name);
    visiting_.erase(type_name);
    visited_.insert(type_name);
    
    return has_cycle;
}

bool TypeChecker::check_circular_in_type_expr(parser::TypeExprNode* expr, const std::string& current_type) {
    if (auto* struct_type = dynamic_cast<parser::StructTypeNode*>(expr)) {
        for (auto& field : struct_type->fields) {
            if (check_circular_in_type_expr(field->type.get(), current_type)) {
                return true;
            }
        }
    } else if (auto* variant_type = dynamic_cast<parser::VariantTypeNode*>(expr)) {
        for (auto& alt : variant_type->alternatives) {
            if (alt->type && check_circular_in_type_expr(alt->type.get(), current_type)) {
                return true;
            }
        }
    } else if (auto* container_type = dynamic_cast<parser::ContainerTypeNode*>(expr)) {
        if (container_type->element_type && check_circular_in_type_expr(container_type->element_type.get(), current_type)) {
            return true;
        }
        if (container_type->key_type && check_circular_in_type_expr(container_type->key_type.get(), current_type)) {
            return true;
        }
        if (container_type->value_type && check_circular_in_type_expr(container_type->value_type.get(), current_type)) {
            return true;
        }
    } else if (auto* id_type = dynamic_cast<parser::IdentifierTypeNode*>(expr)) {
        if (id_type->name == current_type) {
            return true;  // Direct self-reference
        }
        
        if (visiting_.count(id_type->name) > 0) {
            return true;  // Cycle detected
        }
        
        if (visited_.count(id_type->name) > 0) {
            return false;  // Already checked, no cycle
        }
        
        auto it = symbol_table_.find(id_type->name);
        if (it != symbol_table_.end()) {
            visiting_.insert(id_type->name);
            bool has_cycle = check_circular_in_type_expr(it->second->type.get(), current_type);
            visiting_.erase(id_type->name);
            visited_.insert(id_type->name);
            return has_cycle;
        }
    } else if (dynamic_cast<parser::RefTypeNode*>(expr)) {
        // ref<entity> breaks circular dependencies
        return false;
    }
    // Primitive types don't cause cycles
    
    return false;
}

bool TypeChecker::is_type_defined(const std::string& type_name) const {
    return symbol_table_.count(type_name) > 0;
}

bool TypeChecker::is_primitive_type(parser::TypeExprNode* expr) const {
    return dynamic_cast<parser::PrimitiveTypeNode*>(expr) != nullptr;
}

bool TypeChecker::is_leaf_type(parser::TypeExprNode* expr) const {
    return is_primitive_type(expr) || 
           dynamic_cast<parser::RefTypeNode*>(expr) != nullptr ||
           dynamic_cast<parser::EnumTypeNode*>(expr) != nullptr;
}

void TypeChecker::check_leaf_nodes(parser::TypeExprNode* expr, const std::string& context, bool must_terminate) {
    if (auto* struct_type = dynamic_cast<parser::StructTypeNode*>(expr)) {
        for (auto& field : struct_type->fields) {
            check_leaf_nodes(field->type.get(), context + "." + field->name, true);
        }
    } else if (auto* variant_type = dynamic_cast<parser::VariantTypeNode*>(expr)) {
        for (auto& alt : variant_type->alternatives) {
            if (alt->type) {
                check_leaf_nodes(alt->type.get(), context + "." + alt->name, true);
            }
        }
    } else if (auto* container_type = dynamic_cast<parser::ContainerTypeNode*>(expr)) {
        if (container_type->element_type) {
            check_leaf_nodes(container_type->element_type.get(), context, must_terminate);
        }
        if (container_type->key_type) {
            check_leaf_nodes(container_type->key_type.get(), context + " (key)", must_terminate);
        }
        if (container_type->value_type) {
            check_leaf_nodes(container_type->value_type.get(), context + " (value)", must_terminate);
        }
    } else if (auto* id_type = dynamic_cast<parser::IdentifierTypeNode*>(expr)) {
        // Follow the reference and check it
        auto it = symbol_table_.find(id_type->name);
        if (it != symbol_table_.end()) {
            check_leaf_nodes(it->second->type.get(), id_type->name, must_terminate);
        }
    } else if (must_terminate && !is_leaf_type(expr)) {
        // We've reached a non-leaf type where we expected termination
        report_error("Type path in '" + context + "' does not terminate at a primitive or ref type", expr);
    }
    // Primitive types and ref types are valid leaves - no error
}

void TypeChecker::report_error(const std::string& message) {
    errors_.push_back(message);
}

void TypeChecker::report_error(const std::string& message, uint32_t line, uint32_t column) {
    std::ostringstream oss;
    oss << "Line " << line << ", Column " << column << ": " << message;
    errors_.push_back(oss.str());
}

void TypeChecker::report_error(const std::string& message, parser::ASTNode* node) {
    report_error(message, node->line, node->column);
}

} // namespace semantic
} // namespace carch

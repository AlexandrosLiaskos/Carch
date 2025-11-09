#pragma once

#include "../parser/ast.h"
#include <string>
#include <sstream>
#include <unordered_set>
#include <memory>

namespace carch {
namespace codegen {

struct GenerationOptions {
    std::string namespace_name = "game";
    std::string output_basename = "generated";
    bool generate_serialization = false;
    bool generate_reflection = false;
    bool use_strong_entity_id = true;
    std::string entity_id_typedef = "uint64_t";
    int indentation_size = 4;
};

class CppGenerator {
public:
    explicit CppGenerator(parser::SchemaNode* schema, const GenerationOptions& options = GenerationOptions{});
    
    // Generate C++ header file
    std::string generate_header();
    
    // Generate C++ source file (if needed for implementations)
    std::string generate_source();

private:
    parser::SchemaNode* schema_;
    GenerationOptions options_;
    int current_indent_;
    std::unordered_set<std::string> generated_includes_;
    
    // Generation methods
    std::string generate_includes();
    std::string generate_namespace_open();
    std::string generate_namespace_close();
    std::string generate_type_definition(parser::TypeDefinitionNode* def);
    std::string generate_struct(const std::string& name, parser::StructTypeNode* node);
    std::string generate_variant(const std::string& name, parser::VariantTypeNode* node);
    std::string generate_enum(const std::string& name, parser::EnumTypeNode* node);
    std::string generate_field(parser::FieldNode* field);
    
    std::string map_type(parser::TypeExprNode* expr, const std::string& context = "");
    std::string map_primitive_type(parser::PrimitiveType type);
    std::string map_container_type(parser::ContainerTypeNode* node, const std::string& context = "");
    std::string map_struct_type(parser::StructTypeNode* node);
    std::string map_variant_type(parser::VariantTypeNode* node);
    std::string map_enum_type(parser::EnumTypeNode* node, const std::string& context);
    
    // Track and emit anonymous enums as named types
    std::ostringstream hoisted_types_;
    int anonymous_type_counter_ = 0;
    
    // Utilities
    std::string indent();
    void increase_indent();
    void decrease_indent();
    void add_include(const std::string& include);
    std::string sanitize_name(const std::string& name);
    std::string to_pascal_case(const std::string& name);
    std::string to_screaming_snake_case(const std::string& name);
    std::string generate_header_guard_name();
};

} // namespace codegen
} // namespace carch

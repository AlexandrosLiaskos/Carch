#include "parser/ast.h"
#include <sstream>

namespace carch {
namespace parser {

// Helper for indentation
static std::string indent_str(int indent) {
    return std::string(indent * 2, ' ');
}

std::string SchemaNode::to_string(int indent) const {
    std::ostringstream oss;
    oss << indent_str(indent) << "Schema {\n";
    for (const auto& def : definitions) {
        oss << def->to_string(indent + 1) << "\n";
    }
    oss << indent_str(indent) << "}";
    return oss.str();
}

std::string TypeDefinitionNode::to_string(int indent) const {
    std::ostringstream oss;
    oss << indent_str(indent) << "TypeDef " << name << " : ";
    oss << type->to_string(0);
    return oss.str();
}

std::string StructTypeNode::to_string(int indent) const {
    std::ostringstream oss;
    oss << "struct {";
    for (size_t i = 0; i < fields.size(); ++i) {
        oss << " " << fields[i]->name << ": " << fields[i]->type->to_string(0);
        if (i < fields.size() - 1) oss << ",";
    }
    oss << " }";
    return oss.str();
}

std::string VariantTypeNode::to_string(int indent) const {
    std::ostringstream oss;
    oss << "variant {";
    for (size_t i = 0; i < alternatives.size(); ++i) {
        oss << " " << alternatives[i]->name;
        if (alternatives[i]->type) {
            oss << ": " << alternatives[i]->type->to_string(0);
        }
        if (i < alternatives.size() - 1) oss << ",";
    }
    oss << " }";
    return oss.str();
}

std::string EnumTypeNode::to_string(int indent) const {
    std::ostringstream oss;
    oss << "enum {";
    for (size_t i = 0; i < values.size(); ++i) {
        oss << " " << values[i];
        if (i < values.size() - 1) oss << ",";
    }
    oss << " }";
    return oss.str();
}

std::string FieldNode::to_string(int indent) const {
    return name + ": " + type->to_string(0);
}

std::string AlternativeNode::to_string(int indent) const {
    std::string result = name;
    if (type) {
        result += ": " + type->to_string(0);
    }
    return result;
}

std::string PrimitiveTypeNode::to_string(int indent) const {
    switch (primitive) {
        case PrimitiveType::STR: return "str";
        case PrimitiveType::INT: return "int";
        case PrimitiveType::BOOL: return "bool";
        case PrimitiveType::UNIT: return "unit";
        case PrimitiveType::U8: return "u8";
        case PrimitiveType::U16: return "u16";
        case PrimitiveType::U32: return "u32";
        case PrimitiveType::U64: return "u64";
        case PrimitiveType::I8: return "i8";
        case PrimitiveType::I16: return "i16";
        case PrimitiveType::I32: return "i32";
        case PrimitiveType::I64: return "i64";
        case PrimitiveType::F32: return "f32";
        case PrimitiveType::F64: return "f64";
        default: return "unknown";
    }
}

std::string ContainerTypeNode::to_string(int indent) const {
    std::ostringstream oss;
    if (kind == ContainerKind::ARRAY) {
        oss << "array<" << element_type->to_string(0) << ">";
    } else if (kind == ContainerKind::MAP) {
        oss << "map<" << key_type->to_string(0) << ", " << value_type->to_string(0) << ">";
    } else if (kind == ContainerKind::OPTIONAL) {
        oss << "optional<" << element_type->to_string(0) << ">";
    }
    return oss.str();
}

std::string RefTypeNode::to_string(int indent) const {
    return "ref<entity>";
}

std::string IdentifierTypeNode::to_string(int indent) const {
    return name;
}

} // namespace parser
} // namespace carch

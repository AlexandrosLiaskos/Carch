#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace carch {
namespace parser {

// Forward declarations
class ASTNode;
class SchemaNode;
class TypeDefinitionNode;
class TypeExprNode;
class StructTypeNode;
class VariantTypeNode;
class EnumTypeNode;
class FieldNode;
class AlternativeNode;
class PrimitiveTypeNode;
class ContainerTypeNode;
class RefTypeNode;
class IdentifierTypeNode;

// Visitor pattern support
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(SchemaNode* node) = 0;
    virtual void visit(TypeDefinitionNode* node) = 0;
    virtual void visit(StructTypeNode* node) = 0;
    virtual void visit(VariantTypeNode* node) = 0;
    virtual void visit(EnumTypeNode* node) = 0;
    virtual void visit(FieldNode* node) = 0;
    virtual void visit(AlternativeNode* node) = 0;
    virtual void visit(PrimitiveTypeNode* node) = 0;
    virtual void visit(ContainerTypeNode* node) = 0;
    virtual void visit(RefTypeNode* node) = 0;
    virtual void visit(IdentifierTypeNode* node) = 0;
};

// Base AST node
class ASTNode {
public:
    uint32_t line;
    uint32_t column;
    
    ASTNode(uint32_t ln, uint32_t col) : line(ln), column(col) {}
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor* visitor) = 0;
    virtual std::string to_string(int indent = 0) const = 0;
};

// Schema (root node containing all type definitions)
class SchemaNode : public ASTNode {
public:
    std::vector<std::unique_ptr<TypeDefinitionNode>> definitions;
    
    SchemaNode(uint32_t ln, uint32_t col) : ASTNode(ln, col) {}
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
    std::string to_string(int indent = 0) const override;
};

// Type definition (name : type_expr)
class TypeDefinitionNode : public ASTNode {
public:
    std::string name;
    std::unique_ptr<TypeExprNode> type;
    
    TypeDefinitionNode(const std::string& n, uint32_t ln, uint32_t col)
        : ASTNode(ln, col), name(n) {}
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
    std::string to_string(int indent = 0) const override;
};

// Base class for type expressions
class TypeExprNode : public ASTNode {
public:
    TypeExprNode(uint32_t ln, uint32_t col) : ASTNode(ln, col) {}
};

// Struct type
class StructTypeNode : public TypeExprNode {
public:
    std::vector<std::unique_ptr<FieldNode>> fields;
    
    StructTypeNode(uint32_t ln, uint32_t col) : TypeExprNode(ln, col) {}
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
    std::string to_string(int indent = 0) const override;
};

// Variant type
class VariantTypeNode : public TypeExprNode {
public:
    std::vector<std::unique_ptr<AlternativeNode>> alternatives;
    
    VariantTypeNode(uint32_t ln, uint32_t col) : TypeExprNode(ln, col) {}
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
    std::string to_string(int indent = 0) const override;
};

// Enum type
class EnumTypeNode : public TypeExprNode {
public:
    std::vector<std::string> values;
    
    EnumTypeNode(uint32_t ln, uint32_t col) : TypeExprNode(ln, col) {}
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
    std::string to_string(int indent = 0) const override;
};

// Field in struct
class FieldNode : public ASTNode {
public:
    std::string name;
    std::unique_ptr<TypeExprNode> type;
    
    FieldNode(const std::string& n, uint32_t ln, uint32_t col)
        : ASTNode(ln, col), name(n) {}
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
    std::string to_string(int indent = 0) const override;
};

// Alternative in variant
class AlternativeNode : public ASTNode {
public:
    std::string name;
    std::unique_ptr<TypeExprNode> type;  // nullptr means unit type
    
    AlternativeNode(const std::string& n, uint32_t ln, uint32_t col)
        : ASTNode(ln, col), name(n) {}
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
    std::string to_string(int indent = 0) const override;
};

// Primitive type
enum class PrimitiveType {
    STR, INT, BOOL, UNIT,
    U8, U16, U32, U64,
    I8, I16, I32, I64,
    F32, F64
};

class PrimitiveTypeNode : public TypeExprNode {
public:
    PrimitiveType primitive;
    
    PrimitiveTypeNode(PrimitiveType p, uint32_t ln, uint32_t col)
        : TypeExprNode(ln, col), primitive(p) {}
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
    std::string to_string(int indent = 0) const override;
};

// Container type (array, map, optional)
enum class ContainerKind {
    ARRAY,
    MAP,
    OPTIONAL
};

class ContainerTypeNode : public TypeExprNode {
public:
    ContainerKind kind;
    std::unique_ptr<TypeExprNode> element_type;  // For array and optional
    std::unique_ptr<TypeExprNode> key_type;      // For map
    std::unique_ptr<TypeExprNode> value_type;    // For map
    
    ContainerTypeNode(ContainerKind k, uint32_t ln, uint32_t col)
        : TypeExprNode(ln, col), kind(k) {}
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
    std::string to_string(int indent = 0) const override;
};

// Reference type (ref<entity>)
class RefTypeNode : public TypeExprNode {
public:
    RefTypeNode(uint32_t ln, uint32_t col) : TypeExprNode(ln, col) {}
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
    std::string to_string(int indent = 0) const override;
};

// Identifier (reference to user-defined type)
class IdentifierTypeNode : public TypeExprNode {
public:
    std::string name;
    
    IdentifierTypeNode(const std::string& n, uint32_t ln, uint32_t col)
        : TypeExprNode(ln, col), name(n) {}
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
    std::string to_string(int indent = 0) const override;
};

} // namespace parser
} // namespace carch

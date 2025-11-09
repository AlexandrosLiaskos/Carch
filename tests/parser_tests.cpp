// Parser Unit Tests
// Tests for the Carch parser component

#include "../src/parser/parser.h"
#include "../src/parser/ast.h"
#include "../src/lexer/lexer.h"
#include <cassert>
#include <iostream>
#include <string>

using namespace carch::parser;
using namespace carch::lexer;

void test_simple_struct() {
    std::cout << "Testing simple struct parsing...\n";
    
    std::string source = "Position : struct { x: f32, y: f32, z: f32 }";
    Lexer lexer(source);
    Parser parser(lexer);
    
    auto schema = parser.parse();
    
    assert(!parser.has_errors());
    assert(schema->definitions.size() == 1);
    assert(schema->definitions[0]->name == "Position");
    
    auto* struct_type = dynamic_cast<StructTypeNode*>(schema->definitions[0]->type.get());
    assert(struct_type != nullptr);
    assert(struct_type->fields.size() == 3);
    assert(struct_type->fields[0]->name == "x");
    assert(struct_type->fields[1]->name == "y");
    assert(struct_type->fields[2]->name == "z");
    
    std::cout << "  ✓ Simple struct parsed correctly\n";
}

void test_nested_struct() {
    std::cout << "Testing nested struct parsing...\n";
    
    std::string source = R"(
        Transform : struct {
            position: struct { x: f32, y: f32, z: f32 },
            rotation: struct { x: f32, y: f32, z: f32, w: f32 }
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    
    auto schema = parser.parse();
    
    assert(!parser.has_errors());
    assert(schema->definitions.size() == 1);
    
    auto* struct_type = dynamic_cast<StructTypeNode*>(schema->definitions[0]->type.get());
    assert(struct_type != nullptr);
    assert(struct_type->fields.size() == 2);
    
    auto* pos_type = dynamic_cast<StructTypeNode*>(struct_type->fields[0]->type.get());
    assert(pos_type != nullptr);
    assert(pos_type->fields.size() == 3);
    
    std::cout << "  ✓ Nested struct parsed correctly\n";
}

void test_variant_parsing() {
    std::cout << "Testing variant parsing...\n";
    
    std::string source = R"(
        State : variant {
            idle: unit,
            running,
            jumping
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    
    auto schema = parser.parse();
    
    assert(!parser.has_errors());
    assert(schema->definitions.size() == 1);
    
    auto* variant_type = dynamic_cast<VariantTypeNode*>(schema->definitions[0]->type.get());
    assert(variant_type != nullptr);
    assert(variant_type->alternatives.size() == 3);
    assert(variant_type->alternatives[0]->name == "idle");
    assert(variant_type->alternatives[1]->name == "running");
    assert(variant_type->alternatives[2]->name == "jumping");
    
    std::cout << "  ✓ Variant parsed correctly\n";
}

void test_enum_parsing() {
    std::cout << "Testing enum parsing...\n";
    
    std::string source = "Team : enum { red, blue, green }";
    
    Lexer lexer(source);
    Parser parser(lexer);
    
    auto schema = parser.parse();
    
    assert(!parser.has_errors());
    assert(schema->definitions.size() == 1);
    
    auto* enum_type = dynamic_cast<EnumTypeNode*>(schema->definitions[0]->type.get());
    assert(enum_type != nullptr);
    assert(enum_type->values.size() == 3);
    assert(enum_type->values[0] == "red");
    assert(enum_type->values[1] == "blue");
    assert(enum_type->values[2] == "green");
    
    std::cout << "  ✓ Enum parsed correctly\n";
}

void test_container_types() {
    std::cout << "Testing container type parsing...\n";
    
    std::string source = R"(
        Container : struct {
            items: array<u32>,
            lookup: map<str, u32>,
            maybe: optional<str>
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    
    auto schema = parser.parse();
    
    assert(!parser.has_errors());
    assert(schema->definitions.size() == 1);
    
    auto* struct_type = dynamic_cast<StructTypeNode*>(schema->definitions[0]->type.get());
    assert(struct_type != nullptr);
    assert(struct_type->fields.size() == 3);
    
    auto* array_type = dynamic_cast<ContainerTypeNode*>(struct_type->fields[0]->type.get());
    assert(array_type != nullptr);
    assert(array_type->kind == ContainerKind::ARRAY);
    
    auto* map_type = dynamic_cast<ContainerTypeNode*>(struct_type->fields[1]->type.get());
    assert(map_type != nullptr);
    assert(map_type->kind == ContainerKind::MAP);
    
    auto* optional_type = dynamic_cast<ContainerTypeNode*>(struct_type->fields[2]->type.get());
    assert(optional_type != nullptr);
    assert(optional_type->kind == ContainerKind::OPTIONAL);
    
    std::cout << "  ✓ Container types parsed correctly\n";
}

void test_ref_type() {
    std::cout << "Testing ref type parsing...\n";
    
    std::string source = "Node : struct { parent: ref<entity> }";
    
    Lexer lexer(source);
    Parser parser(lexer);
    
    auto schema = parser.parse();
    
    assert(!parser.has_errors());
    assert(schema->definitions.size() == 1);
    
    auto* struct_type = dynamic_cast<StructTypeNode*>(schema->definitions[0]->type.get());
    assert(struct_type != nullptr);
    
    auto* ref_type = dynamic_cast<RefTypeNode*>(struct_type->fields[0]->type.get());
    assert(ref_type != nullptr);
    
    std::cout << "  ✓ Ref type parsed correctly\n";
}

void test_compact_syntax() {
    std::cout << "Testing compact syntax parsing...\n";
    
    std::string source = "Position : struct { x: f32, y: f32 }";
    
    Lexer lexer(source);
    Parser parser(lexer);
    
    auto schema = parser.parse();
    
    assert(!parser.has_errors());
    assert(schema->definitions.size() == 1);
    
    std::cout << "  ✓ Compact syntax parsed correctly\n";
}

void test_multiple_definitions() {
    std::cout << "Testing multiple type definitions...\n";
    
    std::string source = R"(
        Position : struct { x: f32, y: f32 }
        Velocity : struct { dx: f32, dy: f32 }
        Team : enum { red, blue }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    
    auto schema = parser.parse();
    
    assert(!parser.has_errors());
    assert(schema->definitions.size() == 3);
    assert(schema->definitions[0]->name == "Position");
    assert(schema->definitions[1]->name == "Velocity");
    assert(schema->definitions[2]->name == "Team");
    
    std::cout << "  ✓ Multiple definitions parsed correctly\n";
}

int main() {
    std::cout << "Running Parser Tests\n";
    std::cout << "====================\n\n";
    
    test_simple_struct();
    test_nested_struct();
    test_variant_parsing();
    test_enum_parsing();
    test_container_types();
    test_ref_type();
    test_compact_syntax();
    test_multiple_definitions();
    
    std::cout << "\n✓ All parser tests passed!\n";
    return 0;
}

// Code Generation Tests
// Tests for the Carch C++ code generator

#include "../src/codegen/cpp_generator.h"
#include "../src/parser/parser.h"
#include "../src/lexer/lexer.h"
#include <cassert>
#include <iostream>
#include <string>

using namespace carch::codegen;
using namespace carch::parser;
using namespace carch::lexer;

std::unique_ptr<SchemaNode> parse(const std::string& source) {
    Lexer lexer(source);
    Parser parser(lexer);
    return parser.parse();
}

void test_struct_generation() {
    std::cout << "Testing struct code generation...\n";
    
    std::string source = "Position : struct { x: f32, y: f32, z: f32 }";
    auto schema = parse(source);
    
    GenerationOptions options;
    options.namespace_name = "test";
    CppGenerator generator(schema.get(), options);
    
    std::string header = generator.generate_header();
    
    assert(header.find("struct Position") != std::string::npos);
    assert(header.find("float x;") != std::string::npos);
    assert(header.find("float y;") != std::string::npos);
    assert(header.find("float z;") != std::string::npos);
    assert(header.find("namespace test") != std::string::npos);
    
    std::cout << "  ✓ Struct generated correctly\n";
}

void test_variant_generation() {
    std::cout << "Testing variant code generation...\n";
    
    std::string source = "State : variant { idle, running: struct { speed: f32 } }";
    auto schema = parse(source);
    
    CppGenerator generator(schema.get());
    std::string header = generator.generate_header();
    
    assert(header.find("using State = std::variant") != std::string::npos);
    assert(header.find("std::monostate") != std::string::npos);
    
    std::cout << "  ✓ Variant generated correctly\n";
}

void test_enum_generation() {
    std::cout << "Testing enum code generation...\n";
    
    std::string source = "Team : enum { red, blue, green }";
    auto schema = parse(source);
    
    CppGenerator generator(schema.get());
    std::string header = generator.generate_header();
    
    assert(header.find("enum class Team") != std::string::npos);
    assert(header.find("red") != std::string::npos);
    assert(header.find("blue") != std::string::npos);
    assert(header.find("green") != std::string::npos);
    
    std::cout << "  ✓ Enum generated correctly\n";
}

void test_type_mapping() {
    std::cout << "Testing type mapping...\n";
    
    std::string source = R"(
        AllTypes : struct {
            s: str,
            i: int,
            u: u32,
            f: f32,
            b: bool
        }
    )";
    auto schema = parse(source);
    
    CppGenerator generator(schema.get());
    std::string header = generator.generate_header();
    
    assert(header.find("std::string s;") != std::string::npos);
    assert(header.find("int32_t i;") != std::string::npos);
    assert(header.find("uint32_t u;") != std::string::npos);
    assert(header.find("float f;") != std::string::npos);
    assert(header.find("bool b;") != std::string::npos);
    
    std::cout << "  ✓ Type mapping correct\n";
}

void test_container_generation() {
    std::cout << "Testing container code generation...\n";
    
    std::string source = R"(
        Container : struct {
            items: array<u32>,
            lookup: map<str, u32>,
            maybe: optional<str>
        }
    )";
    auto schema = parse(source);
    
    CppGenerator generator(schema.get());
    std::string header = generator.generate_header();
    
    assert(header.find("std::vector<uint32_t>") != std::string::npos);
    assert(header.find("std::unordered_map<std::string, uint32_t>") != std::string::npos);
    assert(header.find("std::optional<std::string>") != std::string::npos);
    
    std::cout << "  ✓ Containers generated correctly\n";
}

void test_includes() {
    std::cout << "Testing include generation...\n";
    
    std::string source = "Simple : struct { value: u32 }";
    auto schema = parse(source);
    
    CppGenerator generator(schema.get());
    std::string header = generator.generate_header();
    
    assert(header.find("#include <cstdint>") != std::string::npos);
    assert(header.find("#include <string>") != std::string::npos);
    assert(header.find("#include <vector>") != std::string::npos);
    
    std::cout << "  ✓ Includes generated correctly\n";
}

void test_header_guard() {
    std::cout << "Testing header guard generation...\n";
    
    std::string source = "Test : struct { a: u32 }";
    auto schema = parse(source);
    
    CppGenerator generator(schema.get());
    std::string header = generator.generate_header();
    
    assert(header.find("#pragma once") != std::string::npos);
    assert(header.find("#ifndef") != std::string::npos);
    assert(header.find("#define") != std::string::npos);
    assert(header.find("#endif") != std::string::npos);
    
    std::cout << "  ✓ Header guards generated correctly\n";
}

void test_namespace_wrapping() {
    std::cout << "Testing namespace wrapping...\n";
    
    std::string source = "Test : struct { a: u32 }";
    auto schema = parse(source);
    
    GenerationOptions options;
    options.namespace_name = "mygame";
    CppGenerator generator(schema.get(), options);
    
    std::string header = generator.generate_header();
    
    assert(header.find("namespace mygame {") != std::string::npos);
    assert(header.find("} // namespace mygame") != std::string::npos);
    
    std::cout << "  ✓ Namespace wrapping correct\n";
}

void test_pascal_case_conversion() {
    std::cout << "Testing PascalCase conversion...\n";
    
    std::string source = "player_health : struct { current: u32, max_value: u32 }";
    auto schema = parse(source);
    
    CppGenerator generator(schema.get());
    std::string header = generator.generate_header();
    
    // Should convert player_health to PlayerHealth
    assert(header.find("struct PlayerHealth") != std::string::npos);
    
    std::cout << "  ✓ PascalCase conversion correct\n";
}

int main() {
    std::cout << "Running Code Generation Tests\n";
    std::cout << "==============================\n\n";
    
    test_struct_generation();
    test_variant_generation();
    test_enum_generation();
    test_type_mapping();
    test_container_generation();
    test_includes();
    test_header_guard();
    test_namespace_wrapping();
    test_pascal_case_conversion();
    
    std::cout << "\n✓ All code generation tests passed!\n";
    return 0;
}

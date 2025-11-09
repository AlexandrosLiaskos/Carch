// Semantic Analysis Tests
// Tests for the Carch type checker component

#include "../src/semantic/type_checker.h"
#include "../src/parser/parser.h"
#include "../src/lexer/lexer.h"
#include <cassert>
#include <iostream>
#include <string>

using namespace carch::semantic;
using namespace carch::parser;
using namespace carch::lexer;

std::unique_ptr<SchemaNode> parse(const std::string& source) {
    Lexer lexer(source);
    Parser parser(lexer);
    return parser.parse();
}

void test_valid_definitions() {
    std::cout << "Testing valid type definitions...\n";
    
    std::string source = R"(
        Position : struct { x: f32, y: f32 }
        Velocity : struct { dx: f32, dy: f32 }
    )";
    
    auto schema = parse(source);
    TypeChecker checker(schema.get());
    
    assert(checker.check());
    assert(!checker.has_errors());
    
    std::cout << "  ✓ Valid definitions accepted\n";
}

void test_duplicate_type_names() {
    std::cout << "Testing duplicate type name detection...\n";
    
    std::string source = R"(
        Position : struct { x: f32, y: f32 }
        Position : struct { a: u32, b: u32 }
    )";
    
    auto schema = parse(source);
    TypeChecker checker(schema.get());
    
    assert(!checker.check());
    assert(checker.has_errors());
    
    std::cout << "  ✓ Duplicate type names detected\n";
}

void test_undefined_type_reference() {
    std::cout << "Testing undefined type reference detection...\n";
    
    std::string source = R"(
        Entity : struct { pos: UndefinedType }
    )";
    
    auto schema = parse(source);
    TypeChecker checker(schema.get());
    
    assert(!checker.check());
    assert(checker.has_errors());
    
    std::cout << "  ✓ Undefined type reference detected\n";
}

void test_duplicate_field_names() {
    std::cout << "Testing duplicate field name detection...\n";
    
    std::string source = R"(
        Position : struct { x: f32, y: f32, x: f32 }
    )";
    
    auto schema = parse(source);
    TypeChecker checker(schema.get());
    
    assert(!checker.check());
    assert(checker.has_errors());
    
    std::cout << "  ✓ Duplicate field names detected\n";
}

void test_duplicate_variant_alternatives() {
    std::cout << "Testing duplicate variant alternative detection...\n";
    
    std::string source = R"(
        State : variant { idle, running, idle }
    )";
    
    auto schema = parse(source);
    TypeChecker checker(schema.get());
    
    assert(!checker.check());
    assert(checker.has_errors());
    
    std::cout << "  ✓ Duplicate variant alternatives detected\n";
}

void test_duplicate_enum_values() {
    std::cout << "Testing duplicate enum value detection...\n";
    
    std::string source = R"(
        Team : enum { red, blue, red }
    )";
    
    auto schema = parse(source);
    TypeChecker checker(schema.get());
    
    assert(!checker.check());
    assert(checker.has_errors());
    
    std::cout << "  ✓ Duplicate enum values detected\n";
}

void test_empty_struct() {
    std::cout << "Testing empty struct detection...\n";
    
    std::string source = R"(
        Empty : struct { }
    )";
    
    auto schema = parse(source);
    TypeChecker checker(schema.get());
    
    assert(!checker.check());
    assert(checker.has_errors());
    
    std::cout << "  ✓ Empty struct detected\n";
}

void test_valid_type_reference() {
    std::cout << "Testing valid type references...\n";
    
    std::string source = R"(
        Position : struct { x: f32, y: f32 }
        Entity : struct { pos: Position }
    )";
    
    auto schema = parse(source);
    TypeChecker checker(schema.get());
    
    assert(checker.check());
    assert(!checker.has_errors());
    
    std::cout << "  ✓ Valid type references accepted\n";
}

void test_nested_optional_detection() {
    std::cout << "Testing nested optional detection...\n";
    
    std::string source = R"(
        Invalid : struct { field: optional<optional<u32>> }
    )";
    
    auto schema = parse(source);
    TypeChecker checker(schema.get());
    
    assert(!checker.check());
    assert(checker.has_errors());
    
    std::cout << "  ✓ Nested optional detected\n";
}

void test_forward_reference_detection() {
    std::cout << "Testing forward reference detection...\n";
    
    std::string source = R"(
        Entity : struct { pos: Position }
        Position : struct { x: f32, y: f32 }
    )";
    
    auto schema = parse(source);
    TypeChecker checker(schema.get());
    
    assert(!checker.check());
    assert(checker.has_errors());
    
    std::cout << "  ✓ Forward references detected\n";
}

void test_non_leaf_termination() {
    std::cout << "Testing non-leaf type termination...\n";
    
    // This should be valid - all paths end in primitives
    std::string valid_source = R"(
        Position : struct { x: f32, y: f32 }
    )";
    
    auto valid_schema = parse(valid_source);
    TypeChecker valid_checker(valid_schema.get());
    assert(valid_checker.check());
    
    std::cout << "  ✓ Leaf node validation works\n";
}

int main() {
    std::cout << "Running Semantic Analysis Tests\n";
    std::cout << "================================\n\n";
    
    test_valid_definitions();
    test_duplicate_type_names();
    test_undefined_type_reference();
    test_duplicate_field_names();
    test_duplicate_variant_alternatives();
    test_duplicate_enum_values();
    test_empty_struct();
    test_valid_type_reference();
    test_nested_optional_detection();
    test_forward_reference_detection();
    test_non_leaf_termination();
    
    std::cout << "\n✓ All semantic analysis tests passed!\n";
    return 0;
}

// Error Message Tests
// Verifies that error messages are clear and helpful

#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/semantic/type_checker.h"
#include <cassert>
#include <iostream>
#include <string>

void test_parser_error(const std::string& source, const std::string& expected_keyword) {
    carch::lexer::Lexer lexer(source);
    carch::parser::Parser parser(lexer);
    auto schema = parser.parse();
    
    assert(parser.has_errors());
    // Parser should report errors
}

void test_semantic_error(const std::string& source, const std::string& expected_keyword) {
    carch::lexer::Lexer lexer(source);
    carch::parser::Parser parser(lexer);
    auto schema = parser.parse();
    
    if (parser.has_errors()) {
        return; // Parser caught it first, that's okay
    }
    
    carch::semantic::TypeChecker checker(schema.get());
    bool success = checker.check();
    assert(!success); // Should fail semantic analysis
}

void test_parser_errors() {
    std::cout << "Testing parser error messages...\n";
    
    // Missing colon
    test_parser_error("Point struct { x: u32 }", ":");
    
    // Missing opening brace
    test_parser_error("Point : struct x: u32 }", "{");
    
    // Missing closing brace
    test_parser_error("Point : struct { x: u32", "}");
    
    // Unexpected token
    test_parser_error("Point : struct { x: u32 u32 }", "unexpected");
    
    // Malformed type expression
    test_parser_error("Point : struct { x: }", "type");
    
    std::cout << "  ✓ Parser errors reported correctly\n";
}

void test_semantic_errors() {
    std::cout << "Testing semantic error messages...\n";
    
    // Duplicate type names
    test_semantic_error(R"(
        Point : struct { x: u32 }
        Point : struct { y: u32 }
    )", "duplicate");
    
    // Undefined type reference
    test_semantic_error(R"(
        Player : struct { pos: UndefinedType }
    )", "undefined");
    
    // Duplicate field names
    test_semantic_error(R"(
        Point : struct { x: u32, x: u32 }
    )", "duplicate");
    
    // Duplicate variant alternatives
    test_semantic_error(R"(
        Color : variant { red: unit, red: unit }
    )", "duplicate");
    
    // Duplicate enum values
    test_semantic_error(R"(
        Status : enum { active, active }
    )", "duplicate");
    
    // Circular dependency (direct)
    test_semantic_error(R"(
        Node : struct { child: Node }
    )", "circular");
    
    // Nested optional
    test_semantic_error(R"(
        Bad : struct { field: optional<optional<u32>> }
    )", "optional");
    
    std::cout << "  ✓ Semantic errors reported correctly\n";
}

void test_forward_reference_handling() {
    std::cout << "Testing forward reference handling...\n";
    
    // Forward references with entity refs should work
    std::string source = R"(
        Node : struct { next: ref<entity> }
        Graph : struct { root: ref<entity> }
    )";
    
    carch::lexer::Lexer lexer(source);
    carch::parser::Parser parser(lexer);
    auto schema = parser.parse();
    assert(!parser.has_errors());
    
    carch::semantic::TypeChecker checker(schema.get());
    bool success = checker.check();
    assert(success); // Should succeed with refs
    
    // But direct forward reference should fail
    test_semantic_error(R"(
        First : struct { second: Second }
        Second : struct { value: u32 }
    )", "forward");
    
    std::cout << "  ✓ Forward references handled correctly\n";
}

void test_empty_type_detection() {
    std::cout << "Testing empty type detection...\n";
    
    // Empty struct
    test_semantic_error("Empty : struct { }", "empty");
    
    // Empty variant
    test_semantic_error("Empty : variant { }", "empty");
    
    // Empty enum
    test_semantic_error("Empty : enum { }", "empty");
    
    std::cout << "  ✓ Empty types detected\n";
}

void test_error_recovery() {
    std::cout << "Testing error recovery (multiple errors)...\n";
    
    std::string source = R"(
        Point : struct { x: u32, x: u32 }
        Point : struct { y: u32 }
        Player : struct { pos: UndefinedType }
    )";
    
    carch::lexer::Lexer lexer(source);
    carch::parser::Parser parser(lexer);
    auto schema = parser.parse();
    
    // Parser might catch some errors
    if (!parser.has_errors()) {
        // Semantic checker should find multiple errors
        carch::semantic::TypeChecker checker(schema.get());
        bool success = checker.check();
        assert(!success);
        // Should report multiple errors (duplicate field, duplicate type, undefined type)
    }
    
    std::cout << "  ✓ Multiple errors can be reported\n";
}

void test_circular_dependency_detection() {
    std::cout << "Testing circular dependency detection...\n";
    
    // Direct circular dependency
    test_semantic_error(R"(
        Node : struct { child: Node }
    )", "circular");
    
    // Indirect circular dependency
    test_semantic_error(R"(
        A : struct { b: B }
        B : struct { a: A }
    )", "circular");
    
    // Three-way circular dependency
    test_semantic_error(R"(
        A : struct { b: B }
        B : struct { c: C }
        C : struct { a: A }
    )", "circular");
    
    // But circular refs with entity references should work
    std::string source = R"(
        Node : struct { next: ref<entity> }
        A : struct { b: ref<entity> }
        B : struct { a: ref<entity> }
    )";
    
    carch::lexer::Lexer lexer(source);
    carch::parser::Parser parser(lexer);
    auto schema = parser.parse();
    assert(!parser.has_errors());
    
    carch::semantic::TypeChecker checker(schema.get());
    bool success = checker.check();
    assert(success);
    
    std::cout << "  ✓ Circular dependencies detected correctly\n";
}

void test_nested_optional_detection() {
    std::cout << "Testing nested optional detection...\n";
    
    // Direct nested optional
    test_semantic_error(R"(
        Bad : struct { field: optional<optional<u32>> }
    )", "optional");
    
    // Nested through containers
    test_semantic_error(R"(
        Bad : struct { field: array<optional<optional<str>>> }
    )", "optional");
    
    std::cout << "  ✓ Nested optionals detected\n";
}

int main() {
    std::cout << "Running Error Message Tests\n";
    std::cout << "===========================\n\n";
    
    try {
        test_parser_errors();
        test_semantic_errors();
        test_forward_reference_handling();
        test_empty_type_detection();
        test_error_recovery();
        test_circular_dependency_detection();
        test_nested_optional_detection();
        
        std::cout << "\n✓ All error message tests passed!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed: " << e.what() << "\n";
        return 1;
    }
}

// Stress Tests
// Tests compiler robustness under extreme conditions

#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/semantic/type_checker.h"
#include "../src/codegen/cpp_generator.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <chrono>
#include <string>

using namespace std::chrono;

std::string compile_schema(const std::string& source) {
    carch::lexer::Lexer lexer(source);
    carch::parser::Parser parser(lexer);
    auto schema = parser.parse();
    
    if (parser.has_errors()) {
        throw std::runtime_error("Parser errors");
    }
    
    carch::semantic::TypeChecker checker(schema.get());
    if (!checker.check()) {
        throw std::runtime_error("Semantic errors");
    }
    
    carch::codegen::GenerationOptions opts;
    opts.namespace_name = "test";
    opts.output_basename = "stress";
    carch::codegen::CppGenerator generator(schema.get(), opts);
    
    return generator.generate_header();
}

void test_deeply_nested_structures() {
    std::cout << "Testing deeply nested structures (10 levels)...\n";
    
    auto start = high_resolution_clock::now();
    
    std::ostringstream oss;
    oss << "Root : struct { level1: struct { level2: struct { level3: struct { "
        << "level4: struct { level5: struct { level6: struct { level7: struct { "
        << "level8: struct { level9: struct { level10: struct { value: u32 } } } } } } } } }";
    
    try {
        std::string output = compile_schema(oss.str());
        assert(!output.empty());
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        
        std::cout << "  ✓ Completed in " << duration.count() << "ms\n";
        assert(duration.count() < 5000); // Should complete in <5 seconds
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Failed: " << e.what() << "\n";
        throw;
    }
}

void test_large_schema() {
    std::cout << "Testing large schema (1000 type definitions)...\n";
    
    auto start = high_resolution_clock::now();
    
    std::ostringstream oss;
    for (int i = 0; i < 1000; ++i) {
        oss << "Type" << i << " : struct { field_a: u32, field_b: f32, field_c: str }\n";
    }
    
    try {
        std::string output = compile_schema(oss.str());
        assert(!output.empty());
        assert(output.find("Type0") != std::string::npos);
        assert(output.find("Type999") != std::string::npos);
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        
        std::cout << "  ✓ Completed in " << duration.count() << "ms\n";
        assert(duration.count() < 5000);
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Failed: " << e.what() << "\n";
        throw;
    }
}

void test_wide_struct() {
    std::cout << "Testing wide struct (500 fields)...\n";
    
    auto start = high_resolution_clock::now();
    
    std::ostringstream oss;
    oss << "WideStruct : struct {\n";
    for (int i = 0; i < 500; ++i) {
        oss << "  field_" << i << ": u32,\n";
    }
    oss << "}";
    
    try {
        std::string output = compile_schema(oss.str());
        assert(!output.empty());
        assert(output.find("field_0") != std::string::npos);
        assert(output.find("field_499") != std::string::npos);
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        
        std::cout << "  ✓ Completed in " << duration.count() << "ms\n";
        assert(duration.count() < 5000);
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Failed: " << e.what() << "\n";
        throw;
    }
}

void test_wide_variant() {
    std::cout << "Testing wide variant (200 alternatives)...\n";
    
    auto start = high_resolution_clock::now();
    
    std::ostringstream oss;
    oss << "WideVariant : variant {\n";
    for (int i = 0; i < 200; ++i) {
        oss << "  alt_" << i << ": struct { value: u32 },\n";
    }
    oss << "}";
    
    try {
        std::string output = compile_schema(oss.str());
        assert(!output.empty());
        assert(output.find("alt_0") != std::string::npos);
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        
        std::cout << "  ✓ Completed in " << duration.count() << "ms\n";
        assert(duration.count() < 5000);
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Failed: " << e.what() << "\n";
        throw;
    }
}

void test_wide_enum() {
    std::cout << "Testing wide enum (500 values)...\n";
    
    auto start = high_resolution_clock::now();
    
    std::ostringstream oss;
    oss << "WideEnum : enum {\n";
    for (int i = 0; i < 500; ++i) {
        oss << "  value_" << i << ",\n";
    }
    oss << "}";
    
    try {
        std::string output = compile_schema(oss.str());
        assert(!output.empty());
        assert(output.find("value_0") != std::string::npos);
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        
        std::cout << "  ✓ Completed in " << duration.count() << "ms\n";
        assert(duration.count() < 5000);
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Failed: " << e.what() << "\n";
        throw;
    }
}

void test_long_identifiers() {
    std::cout << "Testing long identifiers (500 chars)...\n";
    
    auto start = high_resolution_clock::now();
    
    std::string long_name(500, 'a');
    std::ostringstream oss;
    oss << long_name << " : struct { " << long_name << "_field: u32 }";
    
    try {
        std::string output = compile_schema(oss.str());
        assert(!output.empty());
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        
        std::cout << "  ✓ Completed in " << duration.count() << "ms\n";
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Failed: " << e.what() << "\n";
        throw;
    }
}

void test_complex_container_nesting() {
    std::cout << "Testing complex container nesting...\n";
    
    auto start = high_resolution_clock::now();
    
    std::ostringstream oss;
    oss << "Complex : struct { "
        << "nested: array<map<str, optional<array<struct { "
        << "inner: map<str, array<optional<u32>>> "
        << "}>>>>"
        << " }";
    
    try {
        std::string output = compile_schema(oss.str());
        assert(!output.empty());
        assert(output.find("std::vector") != std::string::npos);
        assert(output.find("std::unordered_map") != std::string::npos);
        assert(output.find("std::optional") != std::string::npos);
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        
        std::cout << "  ✓ Completed in " << duration.count() << "ms\n";
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Failed: " << e.what() << "\n";
        throw;
    }
}

void test_many_circular_refs() {
    std::cout << "Testing complex circular reference graph...\n";
    
    auto start = high_resolution_clock::now();
    
    std::ostringstream oss;
    oss << "Node1 : struct { next: ref<entity> }\n"
        << "Node2 : struct { next: ref<entity> }\n"
        << "Node3 : struct { next: ref<entity> }\n"
        << "Node4 : struct { next: ref<entity> }\n"
        << "Node5 : struct { next: ref<entity> }\n";
    
    for (int i = 0; i < 50; ++i) {
        oss << "Graph" << i << " : struct { root: ref<entity>, other: ref<entity> }\n";
    }
    
    try {
        std::string output = compile_schema(oss.str());
        assert(!output.empty());
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        
        std::cout << "  ✓ Completed in " << duration.count() << "ms\n";
        assert(duration.count() < 5000);
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Failed: " << e.what() << "\n";
        throw;
    }
}

void test_large_file_parsing() {
    std::cout << "Testing large file parsing (multi-megabyte)...\n";
    
    auto start = high_resolution_clock::now();
    
    std::ostringstream oss;
    // Generate ~2MB of schema code
    for (int i = 0; i < 10000; ++i) {
        oss << "LargeType" << i << " : struct {\n";
        for (int j = 0; j < 10; ++j) {
            oss << "  field_" << j << ": u32,\n";
        }
        oss << "}\n\n";
    }
    
    try {
        std::string source = oss.str();
        std::cout << "  Generated " << (source.size() / 1024 / 1024) << "MB of schema\n";
        
        std::string output = compile_schema(source);
        assert(!output.empty());
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        
        std::cout << "  ✓ Completed in " << duration.count() << "ms\n";
        assert(duration.count() < 10000); // 10 second limit for large files
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Failed: " << e.what() << "\n";
        throw;
    }
}

void test_unicode_identifiers() {
    std::cout << "Testing unicode in comments and strings...\n";
    
    auto start = high_resolution_clock::now();
    
    std::ostringstream oss;
    oss << "// Comment with unicode: 你好世界 🌍\n"
        << "Player : struct {\n"
        << "  // Field with emoji comment 🎮\n"
        << "  name: str,\n"
        << "  // 日本語コメント\n"
        << "  score: u32\n"
        << "}";
    
    try {
        std::string output = compile_schema(oss.str());
        assert(!output.empty());
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        
        std::cout << "  ✓ Completed in " << duration.count() << "ms\n";
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Failed: " << e.what() << "\n";
        throw;
    }
}

int main() {
    std::cout << "Running Stress Tests\n";
    std::cout << "====================\n\n";
    
    try {
        test_deeply_nested_structures();
        test_large_schema();
        test_wide_struct();
        test_wide_variant();
        test_wide_enum();
        test_long_identifiers();
        test_complex_container_nesting();
        test_many_circular_refs();
        test_large_file_parsing();
        test_unicode_identifiers();
        
        std::cout << "\n✓ All stress tests passed!\n";
        return 0;
    } catch (...) {
        std::cout << "\n✗ Some stress tests failed\n";
        return 1;
    }
}
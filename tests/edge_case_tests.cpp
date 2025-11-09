// Edge Case Tests
// Tests for corner cases and unusual input scenarios

#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/semantic/type_checker.h"
#include "../src/codegen/cpp_generator.h"
#include <cassert>
#include <iostream>
#include <string>

std::string compile_schema(const std::string& source, bool should_succeed = true) {
    carch::lexer::Lexer lexer(source);
    carch::parser::Parser parser(lexer);
    auto schema = parser.parse();
    
    if (parser.has_errors()) {
        if (should_succeed) {
            throw std::runtime_error("Unexpected parser errors");
        }
        return "";
    }
    
    carch::semantic::TypeChecker checker(schema.get());
    if (!checker.check()) {
        if (should_succeed) {
            throw std::runtime_error("Unexpected semantic errors");
        }
        return "";
    }
    
    carch::codegen::GenerationOptions opts;
    opts.namespace_name = "test";
    opts.output_basename = "edge";
    carch::codegen::CppGenerator generator(schema.get(), opts);
    
    return generator.generate_header();
}

void test_empty_file() {
    std::cout << "Testing empty file...\n";
    
    std::string output = compile_schema("");
    assert(!output.empty()); // Should generate valid header with just includes/guards
    
    std::cout << "  ✓ Empty file handled correctly\n";
}

void test_only_comments() {
    std::cout << "Testing file with only comments...\n";
    
    std::string source = R"(
        // This is a comment
        // Another comment
        /* Block comment */
    )";
    
    std::string output = compile_schema(source);
    assert(!output.empty());
    
    std::cout << "  ✓ Comment-only file handled correctly\n";
}

void test_trailing_commas() {
    std::cout << "Testing trailing commas...\n";
    
    std::string source = R"(
        Point : struct {
            x: f32,
            y: f32,
        }
        
        Color : variant {
            red: unit,
            green: unit,
            blue: unit,
        }
        
        Status : enum {
            active,
            inactive,
        }
    )";
    
    std::string output = compile_schema(source);
    assert(output.find("Point") != std::string::npos);
    assert(output.find("Color") != std::string::npos);
    assert(output.find("Status") != std::string::npos);
    
    std::cout << "  ✓ Trailing commas accepted\n";
}

void test_mixed_line_endings() {
    std::cout << "Testing mixed line endings...\n";
    
    // Mix CRLF, LF, and CR
    std::string source = "Point : struct {\r\n  x: f32,\n  y: f32\r}\n";
    
    std::string output = compile_schema(source);
    assert(output.find("Point") != std::string::npos);
    
    std::cout << "  ✓ Mixed line endings handled\n";
}

void test_single_character_identifiers() {
    std::cout << "Testing single character identifiers...\n";
    
    std::string source = R"(
        X : struct { a: u32, b: u32 }
        Y : struct { x: X, y: X }
    )";
    
    std::string output = compile_schema(source);
    assert(output.find("struct X") != std::string::npos);
    assert(output.find("struct Y") != std::string::npos);
    
    std::cout << "  ✓ Single character identifiers work\n";
}

void test_underscore_identifiers() {
    std::cout << "Testing underscore identifiers...\n";
    
    std::string source = R"(
        _private : struct { _field: u32 }
        __internal : struct { __data: u32 }
        _leading_underscore : struct { value: u32 }
    )";
    
    std::string output = compile_schema(source);
    assert(output.find("Private") != std::string::npos || output.find("_private") != std::string::npos);
    
    std::cout << "  ✓ Underscore identifiers work\n";
}

void test_pascal_case_edge_cases() {
    std::cout << "Testing PascalCase conversion edge cases...\n";
    
    std::string source = R"(
        player_health_2 : struct { value: u32 }
        _leading : struct { value: u32 }
        multiple___underscores : struct { value: u32 }
        a_b_c_d_e : struct { value: u32 }
    )";
    
    std::string output = compile_schema(source);
    assert(!output.empty());
    
    std::cout << "  ✓ PascalCase edge cases handled\n";
}

void test_optional_in_containers() {
    std::cout << "Testing optional in containers...\n";
    
    std::string source = R"(
        Container : struct {
            opt_array: array<optional<u32>>,
            opt_map: map<str, optional<str>>,
            array_opt: optional<array<u32>>
        }
    )";
    
    std::string output = compile_schema(source);
    assert(output.find("std::vector") != std::string::npos);
    assert(output.find("std::optional") != std::string::npos);
    assert(output.find("std::unordered_map") != std::string::npos);
    
    std::cout << "  ✓ Optional in containers works\n";
}

void test_ref_in_containers() {
    std::cout << "Testing entity refs in containers...\n";
    
    std::string source = R"(
        Container : struct {
            refs: array<ref<Entity>>,
            opt_ref: optional<ref<Entity>>
        }
        Entity : struct { id: u64 }
    )";
    
    std::string output = compile_schema(source);
    assert(output.find("std::vector") != std::string::npos);
    assert(output.find("uint64_t") != std::string::npos);
    
    std::cout << "  ✓ Entity refs in containers work\n";
}

void test_inline_anonymous_types() {
    std::cout << "Testing deeply nested inline types...\n";
    
    std::string source = R"(
        Root : struct {
            nested: struct {
                deep: struct {
                    deeper: struct {
                        deepest: variant {
                            a: struct { x: u32 },
                            b: enum { val1, val2 }
                        }
                    }
                }
            }
        }
    )";
    
    std::string output = compile_schema(source);
    assert(output.find("Root") != std::string::npos);
    
    std::cout << "  ✓ Deeply nested inline types work\n";
}

void test_all_primitive_types() {
    std::cout << "Testing all 14 primitive types...\n";
    
    std::string source = R"(
        AllTypes : struct {
            a: u8,
            b: u16,
            c: u32,
            d: u64,
            e: i8,
            f: i16,
            g: i32,
            h: i64,
            i: f32,
            j: f64,
            k: bool,
            l: str,
            m: bytes,
            n: unit
        }
    )";
    
    std::string output = compile_schema(source);
    assert(output.find("uint8_t") != std::string::npos);
    assert(output.find("uint16_t") != std::string::npos);
    assert(output.find("uint32_t") != std::string::npos);
    assert(output.find("uint64_t") != std::string::npos);
    assert(output.find("int8_t") != std::string::npos);
    assert(output.find("int16_t") != std::string::npos);
    assert(output.find("int32_t") != std::string::npos);
    assert(output.find("int64_t") != std::string::npos);
    assert(output.find("float") != std::string::npos);
    assert(output.find("double") != std::string::npos);
    assert(output.find("bool") != std::string::npos);
    assert(output.find("std::string") != std::string::npos);
    assert(output.find("std::vector<uint8_t>") != std::string::npos);
    
    std::cout << "  ✓ All primitive types work correctly\n";
}

void test_whitespace_variations() {
    std::cout << "Testing whitespace variations...\n";
    
    std::string source = "Point:struct{x:u32,y:u32}";
    std::string output1 = compile_schema(source);
    assert(output1.find("Point") != std::string::npos);
    
    source = "Point   :   struct   {   x  :  u32  ,  y  :  u32  }";
    std::string output2 = compile_schema(source);
    assert(output2.find("Point") != std::string::npos);
    
    source = "Point\t:\tstruct\t{\tx\t:\tu32,\ty\t:\tu32\t}";
    std::string output3 = compile_schema(source);
    assert(output3.find("Point") != std::string::npos);
    
    std::cout << "  ✓ Whitespace variations handled\n";
}

void test_keyword_like_identifiers() {
    std::cout << "Testing keyword-like identifiers...\n";
    
    std::string source = R"(
        structure : struct { field: u32 }
        variant_type : struct { value: u32 }
        enumeration : struct { item: u32 }
    )";
    
    std::string output = compile_schema(source);
    assert(!output.empty());
    
    std::cout << "  ✓ Keyword-like identifiers work\n";
}

void test_empty_inline_types() {
    std::cout << "Testing edge cases with minimal types...\n";
    
    std::string source = R"(
        SingleField : struct { x: u32 }
        SingleAlt : variant { only: unit }
        SingleValue : enum { one }
    )";
    
    std::string output = compile_schema(source);
    assert(output.find("SingleField") != std::string::npos);
    assert(output.find("SingleAlt") != std::string::npos);
    assert(output.find("SingleValue") != std::string::npos);
    
    std::cout << "  ✓ Minimal types work\n";
}

int main() {
    std::cout << "Running Edge Case Tests\n";
    std::cout << "=======================\n\n";
    
    try {
        test_empty_file();
        test_only_comments();
        test_trailing_commas();
        test_mixed_line_endings();
        test_single_character_identifiers();
        test_underscore_identifiers();
        test_pascal_case_edge_cases();
        test_optional_in_containers();
        test_ref_in_containers();
        test_inline_anonymous_types();
        test_all_primitive_types();
        test_whitespace_variations();
        test_keyword_like_identifiers();
        test_empty_inline_types();
        
        std::cout << "\n✓ All edge case tests passed!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed: " << e.what() << "\n";
        return 1;
    }
}

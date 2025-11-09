// Integration Tests
// End-to-end tests for the complete Carch compilation pipeline

#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/semantic/type_checker.h"
#include "../src/codegen/cpp_generator.h"
#include <cassert>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;

std::string read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

void write_file(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to write file: " + path);
    }
    file << content;
}

bool compile_file(const std::string& input_file, const std::string& output_dir, const std::string& namespace_name = "game") {
    try {
        std::string source = read_file(input_file);
        carch::lexer::Lexer lexer(source);
        carch::parser::Parser parser(lexer);
        auto schema = parser.parse();
        
        if (parser.has_errors()) return false;
        
        carch::semantic::TypeChecker checker(schema.get());
        if (!checker.check()) return false;
        
        fs::path input_path(input_file);
        std::string base_name = input_path.stem().string();
        
        carch::codegen::GenerationOptions opts;
        opts.namespace_name = namespace_name;
        opts.output_basename = base_name;
        carch::codegen::CppGenerator generator(schema.get(), opts);
        
        std::string header = generator.generate_header();
        std::string output_file = output_dir + "/" + base_name + ".h";
        write_file(output_file, header);
        
        return true;
    } catch (...) {
        return false;
    }
}

bool try_compile_header(const std::string& header_path) {
    // Try to compile a test program including the header
    std::string test_cpp = header_path + "_test.cpp";
    std::ofstream test_file(test_cpp);
    test_file << "#include \"" << fs::path(header_path).filename().string() << "\"\n";
    test_file << "int main() { return 0; }\n";
    test_file.close();
    
    // Try to compile with g++ or clang++ if available
    std::string compile_cmd = "g++ -std=c++17 -fsyntax-only -I" + 
                             fs::path(header_path).parent_path().string() + 
                             " " + test_cpp + " 2>&1";
    int result = std::system(compile_cmd.c_str());
    
    // Clean up test file
    fs::remove(test_cpp);
    
    return result == 0;
}

void test_basic_compilation() {
    std::cout << "Testing basic compilation pipeline...\n";
    
    // Create test input file
    std::string test_dir = "test_output";
    fs::create_directories(test_dir);
    
    std::string input_file = test_dir + "/test_basic.carch";
    std::ofstream out(input_file);
    out << "Position : struct { x: f32, y: f32, z: f32 }";
    out.close();
    
    // Compile
    bool success = compile_file(input_file, test_dir);
    assert(success);
    
    // Check output file exists
    std::string output_file = test_dir + "/test_basic.h";
    assert(fs::exists(output_file));
    
    // Verify content
    std::string content = read_file(output_file);
    assert(content.find("struct Position") != std::string::npos);
    assert(content.find("float x;") != std::string::npos);
    
    // Try to validate C++ syntax (optional - may not work in all environments)
    // Uncomment if you have a C++ compiler available
    // bool compiles = try_compile_header(output_file);
    // if (!compiles) std::cout << "  Warning: Generated header may have syntax issues\n";
    
    // Cleanup
    fs::remove_all(test_dir);
    
    std::cout << "  ✓ Basic compilation successful\n";
}

void test_complex_entity_compilation() {
    std::cout << "Testing complex entity compilation...\n";
    
    std::string test_dir = "test_output";
    fs::create_directories(test_dir);
    
    std::string input_file = test_dir + "/test_complex.carch";
    std::ofstream out(input_file);
    out << R"(
        Transform : struct {
            position: struct { x: f32, y: f32, z: f32 },
            rotation: struct { x: f32, y: f32, z: f32, w: f32 }
        }
        
        Health : struct { current: u32, max: u32 }
        
        Player : struct {
            id: u64,
            transform: Transform,
            health: Health
        }
    )";
    out.close();
    
    bool success = compile_file(input_file, test_dir);
    assert(success);
    
    std::string output_file = test_dir + "/test_complex.h";
    assert(fs::exists(output_file));
    
    std::string content = read_file(output_file);
    assert(content.find("struct Transform") != std::string::npos);
    assert(content.find("struct Health") != std::string::npos);
    assert(content.find("struct Player") != std::string::npos);
    
    fs::remove_all(test_dir);
    
    std::cout << "  ✓ Complex entity compilation successful\n";
}

void test_error_handling() {
    std::cout << "Testing error handling...\n";
    
    std::string test_dir = "test_output";
    fs::create_directories(test_dir);
    
    // Test duplicate type names
    std::string input_file = test_dir + "/test_error.carch";
    std::ofstream out(input_file);
    out << R"(
        Position : struct { x: f32, y: f32 }
        Position : struct { a: u32, b: u32 }
    )";
    out.close();
    
    bool success = compile_file(input_file, test_dir);
    assert(!success);  // Should fail
    
    fs::remove_all(test_dir);
    
    std::cout << "  ✓ Error handling works correctly\n";
}

void test_variant_compilation() {
    std::cout << "Testing variant compilation...\n";
    
    std::string test_dir = "test_output";
    fs::create_directories(test_dir);
    
    std::string input_file = test_dir + "/test_variant.carch";
    std::ofstream out(input_file);
    out << R"(
        Weapon : variant {
            sword: struct { damage: u32, durability: u32 },
            bow: struct { damage: u32, arrows: u32 },
            unarmed: unit
        }
    )";
    out.close();
    
    bool success = compile_file(input_file, test_dir);
    assert(success);
    
    std::string output_file = test_dir + "/test_variant.h";
    assert(fs::exists(output_file));
    
    std::string content = read_file(output_file);
    assert(content.find("std::variant") != std::string::npos);
    fs::remove_all(test_dir);
    
    std::cout << "  ✓ Variant compilation successful\n";
}

void test_containers_compilation() {
    std::cout << "Testing containers compilation...\n";
    
    std::string test_dir = "test_output";
    fs::create_directories(test_dir);
    
    std::string input_file = test_dir + "/test_containers.carch";
    std::ofstream out(input_file);
    out << R"(
        Inventory : struct {
            items: array<u32>,
            metadata: map<str, str>,
            optional_data: optional<u64>
        }
    )";
    out.close();
    
    bool success = compile_file(input_file, test_dir);
    assert(success);
    
    std::string output_file = test_dir + "/test_containers.h";
    assert(fs::exists(output_file));
    
    std::string content = read_file(output_file);
    assert(content.find("std::vector") != std::string::npos);
    assert(content.find("std::unordered_map") != std::string::npos);
    assert(content.find("std::optional") != std::string::npos);
    
    fs::remove_all(test_dir);
    
    std::cout << "  ✓ Containers compilation successful\n";
}

int main() {
    std::cout << "Running Integration Tests\n";
    std::cout << "==========================\n\n";
    
    test_basic_compilation();
    test_complex_entity_compilation();
    test_error_handling();
    test_variant_compilation();
    test_containers_compilation();
    
    std::cout << "\n✓ All integration tests passed!\n";
    return 0;
}

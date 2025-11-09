// Example Compilation Tests
// Tests that compile all example .carch files and verify output

#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/semantic/type_checker.h"
#include "../src/codegen/cpp_generator.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

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

std::string compile_carch_file(const std::string& input_file) {
    std::string source = read_file(input_file);
    
    carch::lexer::Lexer lexer(source);
    carch::parser::Parser parser(lexer);
    auto schema = parser.parse();
    
    if (parser.has_errors()) {
        throw std::runtime_error("Parser errors occurred");
    }
    
    carch::semantic::TypeChecker checker(schema.get());
    if (!checker.check()) {
        throw std::runtime_error("Semantic errors occurred");
    }
    
    fs::path input_path(input_file);
    std::string base_name = input_path.stem().string();
    
    carch::codegen::GenerationOptions opts;
    opts.namespace_name = "game";
    opts.output_basename = base_name;
    
    carch::codegen::CppGenerator generator(schema.get(), opts);
    return generator.generate_header();
}

void test_basic_carch_compilation() {
    std::cout << "Testing basic.carch compilation...\n";
    
    std::string input_file = "examples/basic.carch";
    if (!fs::exists(input_file)) {
        std::cout << "  ⚠ Skipping - examples/basic.carch not found\n";
        return;
    }
    
    try {
        std::string output = compile_carch_file(input_file);
        
        // Verify all expected types are present
        assert(output.find("Position") != std::string::npos);
        assert(output.find("PersonCompact") != std::string::npos);
        assert(output.find("enum class Gender") != std::string::npos);
        assert(output.find("Contact") != std::string::npos); // variant
        assert(output.find("Health") != std::string::npos);
        assert(output.find("Marker") != std::string::npos);
        assert(output.find("enum class Rarity") != std::string::npos);
        assert(output.find("enum class Direction") != std::string::npos);
        assert(output.find("enum class Team") != std::string::npos);
        assert(output.find("State") != std::string::npos); // variant
        
        std::cout << "  ✓ basic.carch compiled successfully\n";
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Failed to compile basic.carch: " << e.what() << "\n";
        throw;
    }
}

void test_game_entities_carch_compilation() {
    std::cout << "Testing game_entities.carch compilation...\n";
    
    std::string input_file = "examples/game_entities.carch";
    if (!fs::exists(input_file)) {
        std::cout << "  ⚠ Skipping - examples/game_entities.carch not found\n";
        return;
    }
    
    try {
        std::string output = compile_carch_file(input_file);
        
        // Verify complex types
        assert(output.find("struct Transform") != std::string::npos);
        assert(output.find("struct Health") != std::string::npos);
        assert(output.find("struct Inventory") != std::string::npos);
        assert(output.find("Weapon") != std::string::npos); // variant
        assert(output.find("sword") != std::string::npos);
        assert(output.find("bow") != std::string::npos);
        assert(output.find("staff") != std::string::npos);
        assert(output.find("AIState") != std::string::npos); // variant
        assert(output.find("enum class Team") != std::string::npos);
        assert(output.find("struct Player") != std::string::npos);
        assert(output.find("struct Enemy") != std::string::npos);
        assert(output.find("struct Projectile") != std::string::npos);
        assert(output.find("struct NPC") != std::string::npos);
        
        std::cout << "  ✓ game_entities.carch compiled successfully\n";
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Failed to compile game_entities.carch: " << e.what() << "\n";
        throw;
    }
}

void test_advanced_carch_compilation() {
    std::cout << "Testing advanced.carch compilation...\n";
    
    std::string input_file = "examples/advanced.carch";
    if (!fs::exists(input_file)) {
        std::cout << "  ⚠ Skipping - examples/advanced.carch not found\n";
        return;
    }
    
    try {
        std::string output = compile_carch_file(input_file);
        
        // Verify all advanced features
        assert(output.find("AllPrimitives") != std::string::npos);
        assert(output.find("AllContainers") != std::string::npos);
        assert(output.find("SceneGraph") != std::string::npos);
        assert(output.find("DeeplyNested") != std::string::npos);
        assert(output.find("SpellEffect") != std::string::npos); // complex variant
        assert(output.find("Quest") != std::string::npos);
        assert(output.find("MixedSyntax") != std::string::npos);
        assert(output.find("AnimationSystem") != std::string::npos);
        assert(output.find("Configuration") != std::string::npos);
        assert(output.find("Database") != std::string::npos);
        assert(output.find("Tree") != std::string::npos);
        assert(output.find("GameState") != std::string::npos);
        
        // Verify containers are present
        assert(output.find("std::vector") != std::string::npos);
        assert(output.find("std::unordered_map") != std::string::npos);
        assert(output.find("std::optional") != std::string::npos);
        
        std::cout << "  ✓ advanced.carch compiled successfully\n";
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Failed to compile advanced.carch: " << e.what() << "\n";
        throw;
    }
}

int main() {
    std::cout << "Running Example Compilation Tests\n";
    std::cout << "==================================\n\n";
    
    try {
        test_basic_carch_compilation();
        test_game_entities_carch_compilation();
        test_advanced_carch_compilation();
        
        std::cout << "\n✓ All example compilation tests passed!\n";
        return 0;
    } catch (...) {
        std::cout << "\n✗ Some tests failed\n";
        return 1;
    }
}

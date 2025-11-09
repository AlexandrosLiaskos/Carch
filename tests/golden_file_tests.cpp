// Golden File Tests
// Verifies code generation output against pre-approved golden files

#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/semantic/type_checker.h"
#include "../src/codegen/cpp_generator.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
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
    fs::create_directories(fs::path(path).parent_path());
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

std::string normalize_whitespace(const std::string& text) {
    std::istringstream iss(text);
    std::ostringstream oss;
    std::string line;
    
    while (std::getline(iss, line)) {
        // Trim trailing whitespace
        size_t end = line.find_last_not_of(" \t\r\n");
        if (end != std::string::npos) {
            line = line.substr(0, end + 1);
        } else {
            line.clear();
        }
        
        // Skip empty lines in comparison
        if (!line.empty()) {
            oss << line << "\n";
        }
    }
    
    return oss.str();
}

bool compare_generated_with_golden(const std::string& input_file, const std::string& golden_file, bool update_golden = false) {
    std::string generated = compile_carch_file(input_file);
    
    if (update_golden) {
        write_file(golden_file, generated);
        std::cout << "  ✓ Golden file updated: " << golden_file << "\n";
        return true;
    }
    
    if (!fs::exists(golden_file)) {
        std::cerr << "  ✗ Golden file not found: " << golden_file << "\n";
        std::cerr << "    Run with UPDATE_GOLDEN=1 to create it\n";
        return false;
    }
    
    std::string golden = read_file(golden_file);
    
    // Normalize for comparison (ignore minor whitespace differences)
    std::string gen_normalized = normalize_whitespace(generated);
    std::string gold_normalized = normalize_whitespace(golden);
    
    if (gen_normalized == gold_normalized) {
        return true;
    }
    
    // If different, show detailed diff
    std::cerr << "  ✗ Generated output differs from golden file\n";
    std::cerr << "    Golden: " << golden_file << "\n";
    std::cerr << "    To see differences, compare the files manually\n";
    std::cerr << "    To update golden file, run with UPDATE_GOLDEN=1\n";
    
    // Optionally write generated output for comparison
    std::string diff_file = golden_file + ".diff";
    write_file(diff_file, generated);
    std::cerr << "    Generated output saved to: " << diff_file << "\n";
    
    return false;
}

void test_basic_golden() {
    std::cout << "Testing basic.carch against golden file...\n";
    
    std::string input = "examples/basic.carch";
    std::string golden = "tests/golden/basic.h";
    
    if (!fs::exists(input)) {
        std::cout << "  ⚠ Skipping - examples/basic.carch not found\n";
        return;
    }
    
    const char* update_env = std::getenv("UPDATE_GOLDEN");
    bool update = (update_env != nullptr && std::string(update_env) == "1");
    
    bool passed = compare_generated_with_golden(input, golden, update);
    assert(passed);
    
    if (!update) {
        std::cout << "  ✓ basic.carch matches golden file\n";
    }
}

void test_game_entities_golden() {
    std::cout << "Testing game_entities.carch against golden file...\n";
    
    std::string input = "examples/game_entities.carch";
    std::string golden = "tests/golden/game_entities.h";
    
    if (!fs::exists(input)) {
        std::cout << "  ⚠ Skipping - examples/game_entities.carch not found\n";
        return;
    }
    
    const char* update_env = std::getenv("UPDATE_GOLDEN");
    bool update = (update_env != nullptr && std::string(update_env) == "1");
    
    bool passed = compare_generated_with_golden(input, golden, update);
    assert(passed);
    
    if (!update) {
        std::cout << "  ✓ game_entities.carch matches golden file\n";
    }
}

void test_advanced_golden() {
    std::cout << "Testing advanced.carch against golden file...\n";
    
    std::string input = "examples/advanced.carch";
    std::string golden = "tests/golden/advanced.h";
    
    if (!fs::exists(input)) {
        std::cout << "  ⚠ Skipping - examples/advanced.carch not found\n";
        return;
    }
    
    const char* update_env = std::getenv("UPDATE_GOLDEN");
    bool update = (update_env != nullptr && std::string(update_env) == "1");
    
    bool passed = compare_generated_with_golden(input, golden, update);
    assert(passed);
    
    if (!update) {
        std::cout << "  ✓ advanced.carch matches golden file\n";
    }
}

int main() {
    std::cout << "Running Golden File Tests\n";
    std::cout << "=========================\n\n";
    
    const char* update_env = std::getenv("UPDATE_GOLDEN");
    if (update_env && std::string(update_env) == "1") {
        std::cout << "UPDATE_GOLDEN mode: Will regenerate golden files\n\n";
    }
    
    try {
        test_basic_golden();
        test_game_entities_golden();
        test_advanced_golden();
        
        std::cout << "\n✓ All golden file tests passed!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed: " << e.what() << "\n";
        return 1;
    }
}

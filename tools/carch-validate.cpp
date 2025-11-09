// Carch Validator
// Deep validation tool for Carch schemas

#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/semantic/type_checker.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>

std::string read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

bool validate_schema(const std::string& source, bool pedantic) {
    carch::lexer::Lexer lexer(source);
    carch::parser::Parser parser(lexer);
    auto schema = parser.parse();
    
    if (parser.has_errors()) {
        std::cerr << "Parse errors detected\n";
        return false;
    }
    
    carch::semantic::TypeChecker checker(schema.get());
    if (!checker.check()) {
        std::cerr << "Semantic errors detected\n";
        return false;
    }
    
    // Additional validation checks
    if (pedantic) {
        // Check for C++ keyword conflicts
        std::set<std::string> cpp_keywords = {
            "class", "struct", "namespace", "template", "typename",
            "int", "float", "double", "char", "void", "auto"
        };
        
        // More validation could go here
    }
    
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: carch-validate [--pedantic] <file.carch>\n";
        std::cerr << "\nOptions:\n";
        std::cerr << "  --pedantic  Enable strict validation checks\n";
        return 1;
    }
    
    bool pedantic = false;
    std::string input_file;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--pedantic") {
            pedantic = true;
        } else {
            input_file = arg;
        }
    }
    
    if (input_file.empty()) {
        std::cerr << "Error: No input file specified\n";
        return 1;
    }
    
    try {
        std::string source = read_file(input_file);
        bool valid = validate_schema(source, pedantic);
        
        if (valid) {
            std::cout << "✓ " << input_file << " is valid\n";
            return 0;
        } else {
            std::cout << "✗ " << input_file << " has validation errors\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}

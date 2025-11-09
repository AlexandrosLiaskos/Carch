#include "lexer/lexer.h"
#include "parser/parser.h"
#include "semantic/type_checker.h"
#include "codegen/cpp_generator.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstring>

namespace fs = std::filesystem;

struct CommandLineArgs {
    std::vector<std::string> input_files;
    std::string output_dir = "generated";
    std::string namespace_name = "game";
    bool verbose = false;
    bool help = false;
    bool version = false;
};

void print_help() {
    std::cout << "Carch IDL Compiler\n";
    std::cout << "Usage: carch [options] <input-files>\n\n";
    std::cout << "Options:\n";
    std::cout << "  -o, --output <dir>      Output directory (default: generated)\n";
    std::cout << "  -n, --namespace <name>  C++ namespace (default: game)\n";
    std::cout << "  -v, --verbose           Verbose output\n";
    std::cout << "  -h, --help              Show this help message\n";
    std::cout << "  --version               Show version information\n\n";
    std::cout << "Examples:\n";
    std::cout << "  carch schema.carch\n";
    std::cout << "  carch -o output/ -n mygame schema.carch\n";
    std::cout << "  carch *.carch\n";
}

void print_version() {
    std::cout << "Carch IDL Compiler version 0.0.1\n";
    std::cout << "Developer: Alexandros Liaskos\n";
    std::cout << "Repository: https://github.com/AlexandrosLiaskos/Carch\n";
}

CommandLineArgs parse_args(int argc, char* argv[]) {
    CommandLineArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            args.help = true;
        } else if (arg == "--version") {
            args.version = true;
        } else if (arg == "-v" || arg == "--verbose") {
            args.verbose = true;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                args.output_dir = argv[++i];
            } else {
                std::cerr << "Error: " << arg << " requires an argument\n";
                args.help = true;
            }
        } else if (arg == "-n" || arg == "--namespace") {
            if (i + 1 < argc) {
                args.namespace_name = argv[++i];
            } else {
                std::cerr << "Error: " << arg << " requires an argument\n";
                args.help = true;
            }
        } else if (arg[0] == '-') {
            std::cerr << "Unknown option: " << arg << "\n";
            args.help = true;
        } else {
            args.input_files.push_back(arg);
        }
    }
    
    return args;
}

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
    // Create directory if it doesn't exist
    fs::path file_path(path);
    fs::path dir = file_path.parent_path();
    if (!dir.empty() && !fs::exists(dir)) {
        fs::create_directories(dir);
    }
    
    std::ofstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to write file: " + path);
    }
    
    file << content;
}

bool compile_file(const std::string& input_path, const CommandLineArgs& args) {
    if (args.verbose) {
        std::cout << "Compiling: " << input_path << "\n";
    }
    
    try {
        // Read source file
        std::string source = read_file(input_path);
        
        // Lexical analysis
        carch::lexer::Lexer lexer(source);
        if (args.verbose) {
            std::cout << "  [1/4] Lexical analysis...\n";
        }
        
        // Parsing
        carch::parser::Parser parser(lexer);
        if (args.verbose) {
            std::cout << "  [2/4] Parsing...\n";
        }
        auto schema = parser.parse();
        
        if (parser.has_errors()) {
            std::cerr << "Parse errors in " << input_path << ":\n";
            for (const auto& error : parser.errors()) {
                std::cerr << "  " << error << "\n";
            }
            return false;
        }
        
        // Semantic analysis
        if (args.verbose) {
            std::cout << "  [3/4] Semantic analysis...\n";
        }
        carch::semantic::TypeChecker checker(schema.get());
        if (!checker.check()) {
            std::cerr << "Semantic errors in " << input_path << ":\n";
            for (const auto& error : checker.errors()) {
                std::cerr << "  " << error << "\n";
            }
            return false;
        }
        
        // Code generation
        if (args.verbose) {
            std::cout << "  [4/4] Code generation...\n";
        }
        // Extract base name from input file
        fs::path input_file(input_path);
        std::string base_name = input_file.stem().string();
        
        carch::codegen::GenerationOptions gen_opts;
        gen_opts.namespace_name = args.namespace_name;
        gen_opts.output_basename = base_name;
        carch::codegen::CppGenerator generator(schema.get(), gen_opts);
        std::string header = generator.generate_header();
        
        // Determine output file name
        std::string output_path = args.output_dir + "/" + base_name + ".h";
        
        // Write output
        write_file(output_path, header);
        
        if (args.verbose) {
            std::cout << "  Generated: " << output_path << "\n";
        } else {
            std::cout << "Generated: " << output_path << "\n";
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error processing " << input_path << ": " << e.what() << "\n";
        return false;
    }
}

int main(int argc, char* argv[]) {
    CommandLineArgs args = parse_args(argc, argv);
    
    if (args.help) {
        print_help();
        return args.input_files.empty() ? 0 : 1;
    }
    
    if (args.version) {
        print_version();
        return 0;
    }
    
    if (args.input_files.empty()) {
        std::cerr << "Error: No input files specified\n";
        print_help();
        return 1;
    }
    
    bool all_success = true;
    for (const auto& input_file : args.input_files) {
        if (!compile_file(input_file, args)) {
            all_success = false;
        }
    }
    
    return all_success ? 0 : 1;
}

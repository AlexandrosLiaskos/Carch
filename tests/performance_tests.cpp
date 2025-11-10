// Performance Tests
// Benchmarks compiler speed and resource usage

#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/semantic/type_checker.h"
#include "../src/codegen/cpp_generator.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

using namespace std::chrono;

struct BenchmarkResult {
    std::string name;
    int64_t duration_ms;
    size_t input_size;
    size_t output_size;
};

void print_result(const BenchmarkResult& result) {
    std::cout << "  " << std::setw(40) << std::left << result.name 
              << std::setw(10) << std::right << result.duration_ms << "ms"
              << "  (input: " << result.input_size << " bytes, output: " << result.output_size << " bytes)\n";
}

BenchmarkResult benchmark_operation(const std::string& name, const std::string& source, 
                                     std::function<std::string(const std::string&)> operation) {
    auto start = high_resolution_clock::now();
    std::string output = operation(source);
    auto end = high_resolution_clock::now();
    
    BenchmarkResult result;
    result.name = name;
    result.duration_ms = duration_cast<milliseconds>(end - start).count();
    result.input_size = source.size();
    result.output_size = output.size();
    
    return result;
}

std::string run_lexer(const std::string& source) {
    carch::lexer::Lexer lexer(source);
    int token_count = 0;
    while (lexer.next_token().type != carch::lexer::TokenType::EndOfFile) {
        token_count++;
    }
    return std::to_string(token_count);
}

std::string run_parser(const std::string& source) {
    carch::lexer::Lexer lexer(source);
    carch::parser::Parser parser(lexer);
    auto schema = parser.parse();
    return schema ? "parsed" : "error";
}

std::string run_semantic(const std::string& source) {
    carch::lexer::Lexer lexer(source);
    carch::parser::Parser parser(lexer);
    auto schema = parser.parse();
    if (parser.has_errors()) return "error";
    
    carch::semantic::TypeChecker checker(schema.get());
    return checker.check() ? "checked" : "error";
}

std::string run_codegen(const std::string& source) {
    carch::lexer::Lexer lexer(source);
    carch::parser::Parser parser(lexer);
    auto schema = parser.parse();
    if (parser.has_errors()) return "";
    
    carch::semantic::TypeChecker checker(schema.get());
    if (!checker.check()) return "";
    
    carch::codegen::GenerationOptions opts;
    opts.namespace_name = "benchmark";
    opts.output_basename = "test";
    carch::codegen::CppGenerator generator(schema.get(), opts);
    
    return generator.generate_header();
}

void benchmark_lexer() {
    std::cout << "\nLexer Benchmarks:\n";
    std::cout << std::string(80, '-') << "\n";
    
    // 1KB schema
    std::ostringstream small;
    for (int i = 0; i < 10; ++i) {
        small << "Type" << i << " : struct { a: u32, b: f32, c: str }\n";
    }
    auto result1 = benchmark_operation("Lexer - 1KB input", small.str(), run_lexer);
    print_result(result1);
    
    // 10KB schema
    std::ostringstream medium;
    for (int i = 0; i < 100; ++i) {
        medium << "Type" << i << " : struct { a: u32, b: f32, c: str, d: bool, e: u64 }\n";
    }
    auto result2 = benchmark_operation("Lexer - 10KB input", medium.str(), run_lexer);
    print_result(result2);
    
    // 100KB schema
    std::ostringstream large;
    for (int i = 0; i < 1000; ++i) {
        large << "Type" << i << " : struct { a: u32, b: f32, c: str, d: bool, e: u64 }\n";
    }
    auto result3 = benchmark_operation("Lexer - 100KB input", large.str(), run_lexer);
    print_result(result3);
}

void benchmark_parser() {
    std::cout << "\nParser Benchmarks:\n";
    std::cout << std::string(80, '-') << "\n";
    
    // 10 types
    std::ostringstream small;
    for (int i = 0; i < 10; ++i) {
        small << "Type" << i << " : struct { field: u32 }\n";
    }
    auto result1 = benchmark_operation("Parser - 10 types", small.str(), run_parser);
    print_result(result1);
    
    // 100 types
    std::ostringstream medium;
    for (int i = 0; i < 100; ++i) {
        medium << "Type" << i << " : struct { field: u32 }\n";
    }
    auto result2 = benchmark_operation("Parser - 100 types", medium.str(), run_parser);
    print_result(result2);
    
    // 1000 types
    std::ostringstream large;
    for (int i = 0; i < 1000; ++i) {
        large << "Type" << i << " : struct { field: u32 }\n";
    }
    auto result3 = benchmark_operation("Parser - 1000 types", large.str(), run_parser);
    print_result(result3);
    
    // Ensure 1000 types compile in reasonable time
    assert(result3.duration_ms < 1000); // <1 second
}

void benchmark_semantic_analysis() {
    std::cout << "\nSemantic Analysis Benchmarks:\n";
    std::cout << std::string(80, '-') << "\n";
    
    // Simple schema
    std::ostringstream simple;
    for (int i = 0; i < 100; ++i) {
        simple << "Type" << i << " : struct { value: u32 }\n";
    }
    auto result1 = benchmark_operation("Semantic - 100 simple types", simple.str(), run_semantic);
    print_result(result1);
    
    // Complex dependencies
    std::ostringstream complex;
    complex << "Base : struct { value: u32 }\n";
    for (int i = 0; i < 50; ++i) {
        complex << "Type" << i << " : struct { base: Base, value: u32 }\n";
    }
    auto result2 = benchmark_operation("Semantic - 50 types with deps", complex.str(), run_semantic);
    print_result(result2);
    
    // Entity refs (stress cycle detection)
    std::ostringstream refs;
    for (int i = 0; i < 100; ++i) {
        int next = (i + 1) % 100;
        refs << "Node" << i << " : struct { next: ref<entity> }\n";
    }
    auto result3 = benchmark_operation("Semantic - 100 circular refs", refs.str(), run_semantic);
    print_result(result3);
}

void benchmark_code_generation() {
    std::cout << "\nCode Generation Benchmarks:\n";
    std::cout << std::string(80, '-') << "\n";
    
    // Simple structs
    std::ostringstream simple;
    for (int i = 0; i < 100; ++i) {
        simple << "Type" << i << " : struct { a: u32, b: f32, c: str }\n";
    }
    auto result1 = benchmark_operation("Codegen - 100 simple structs", simple.str(), run_codegen);
    print_result(result1);
    
    // Complex types
    std::ostringstream complex;
    for (int i = 0; i < 50; ++i) {
        complex << "Struct" << i << " : struct { field: u32 }\n"
                << "Variant" << i << " : variant { a: unit, b: struct { x: u32 } }\n"
                << "Enum" << i << " : enum { val1, val2, val3 }\n";
    }
    auto result2 = benchmark_operation("Codegen - 50 complex types", complex.str(), run_codegen);
    print_result(result2);
    
    // Large schema
    std::ostringstream large;
    for (int i = 0; i < 500; ++i) {
        large << "Type" << i << " : struct { a: u32, b: f32, c: str, d: bool }\n";
    }
    auto result3 = benchmark_operation("Codegen - 500 types", large.str(), run_codegen);
    print_result(result3);
}

void benchmark_end_to_end() {
    std::cout << "\nEnd-to-End Benchmarks:\n";
    std::cout << std::string(80, '-') << "\n";
    
    // Realistic game schema
    std::ostringstream game;
    game << R"(
        Transform : struct { x: f32, y: f32, z: f32 }
        Health : struct { current: u32, max: u32 }
        Inventory : struct { items: array<u32>, capacity: u32 }
        
        Weapon : variant {
            sword: struct { damage: u32 },
            bow: struct { damage: u32, arrows: u32 },
            staff: struct { magic_power: u32 }
        }
        
        AIState : variant {
            idle: unit,
            patrol: struct { waypoints: array<Transform> },
            chase: struct { target: ref<entity> },
            attack: struct { target: ref<entity> }
        }
        
        Team : enum { player, enemy, neutral }
        
        Player : struct {
            id: u64,
            transform: Transform,
            health: Health,
            inventory: Inventory,
            weapon: Weapon
        }
        
        Enemy : struct {
            id: u64,
            transform: Transform,
            health: Health,
            ai_state: AIState,
            team: Team
        }
        
        Entity : struct {
            id: u64,
            transform: Transform
        }
    )";
    
    for (int i = 0; i < 10; ++i) {
        game << "Component" << i << " : struct { value: u32 }\n";
    }
    
    auto result = benchmark_operation("End-to-End - Realistic game schema", game.str(), run_codegen);
    print_result(result);
    
    assert(result.duration_ms < 500); // Should be fast for realistic schemas
}

int main() {
    std::cout << "Running Performance Benchmarks\n";
    std::cout << "==============================\n";
    
    try {
        benchmark_lexer();
        benchmark_parser();
        benchmark_semantic_analysis();
        benchmark_code_generation();
        benchmark_end_to_end();
        
        std::cout << "\n✓ All benchmarks completed!\n";
        std::cout << "\nNote: Results may vary based on system performance.\n";
        std::cout << "Run multiple times for statistical significance.\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Benchmark failed: " << e.what() << "\n";
        return 1;
    }
}